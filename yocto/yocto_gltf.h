///
/// YOCTO_GLTF: Khronos GLTF loader and writer for Khronos glTF format. Supports
/// all the glTF spec and the Khronos extensions. All parsing and writing code
/// is autogenerated form the schema.
///
/// Supported version and extensions.
/// - glTF v. 2.0 DRAFT
/// - KHR_materials_common (draft); this will be updated to the new draft soon
/// - KHR_binary_glTF
///
/// This library will track the spec which is not only a draft. So expect
/// incompatibilities as the draft evolves. For example, top level dictionaries
/// have move to arrays and PBR is in the core. The library gets updated every
/// time a new draft is avaiable.
///
///
/// USAGE FOR READING:
///
/// 1. load a gltf with load_gltf() for ascii gltf or load_gltf_binary() for the
///       binary version
/// 2. [LOW-LEVEL INTERFACE] access the data directly from the returned object
///   - the data is documented below and matches the GLTF file structure exactly
/// 3. [HIGH-LEVEL INTERFACE] optionally flatten the data as a more friendly
///    representation where shapes are index meshes, supporting points, lines
///    and triangle primitives with flatten_gltf()
///   - the flattened data, documented below, can be use to draw directly on
///   the GPU or in a raytracer
///
/// The interface for each function is described in details in the interface
/// section of this file.
///
/// In the high level interface, shapes are indexed meshes and are described
/// by arrays of vertex indices for points/lines/triangles and arrays for vertex
/// positions, normals, texcoords and colors.
///
///
/// USAGE FOR WRITING:
///
/// 1. include this file (more compilation options below)
/// 2. [LOW-LEVEL INTERFACE] fill a gltf object with your scene data and save
///    the gltf and binary data with save_gltf()
///    ok = save_gltf(filename, obj, error message, flags)
/// 3. [HIGH_LEVEL INTERFACE] create a flattened scene object and turn into a
///    gltf with unflatten_gltf()
///
/// The interface for each function is described in details in the interface
/// section of this file.
///
///
/// COMPILATION:
///
/// To use the library include the .h and compile the .cpp. To use this library
/// as a header-only library, define YGLTF_INLINE before including this file.
///
/// JSON loading depends on json.hpp.
/// Texture loading/saving depends on yocto_image.h.
///
/// If the texture loading/saving dependency is not desired, it can be disabled
/// by defining YGLTF_NO_IMAGE before including this file.
///
///
/// HISTORY:
/// - v 0.6: switch to .h/.cpp pair
/// - v 0.5: bug fixes and added checks for missing files
/// - v 0.4: internally use pointers for performance transaprency
/// - v 0.4: doxygen documentation
/// - v 0.3: load/write interface with exceptions
/// - v 0.2: doxygen comments
/// - v 0.1: bug fix when writing names
/// - v 0.0: initial release
///
namespace ygltf {}

//
// LICENSE:
//
// Copyright (c) 2016 -- 2017 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

//
// LICENSE OF INCLUDED CODE
//
//
// base64.cpp and base64.h
//
// Copyright (C) 2004-2008 René Nyffenegger
//
// This source code is provided 'as-is', without any express or implied
// warranty. In no event will the author be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this source code must not be misrepresented; you must not
// claim that you wrote the original source code. If you use this source code
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original source code.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// René Nyffenegger rene.nyffenegger@adp-gmbh.ch
//

#ifndef _YGLTF_H_
#define _YGLTF_H_

// compilation options
#ifdef YGLTF_INLINE
#define YGLTF_API inline
#else
#define YGLTF_API
#endif

#include <array>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "ext/json.hpp"

// -----------------------------------------------------------------------------
// LOW-LEVEL INTERFACE
// -----------------------------------------------------------------------------

namespace ygltf {

// portable deprecation warnings
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define YGL_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define YGL_DEPRECATED __declspec(deprecated)
#else
#define YGL_DEPRECATED
#endif

///
/// Optional type. Will be sustituted with std::optional once compilers support
/// it.
///
template <typename T>
struct optional {
    /// value
    T value = T();
    /// whether it is valid (use operator bool)
    bool valid = false;

    /// Initialize to empty object
    optional() {}
    /// Initialize to a value
    optional(const T& value, bool valid = true) : value(value), valid(valid) {}

    /// Check if it is valid
    operator bool() const { return valid; }
};

///
/// Json alias
///
using json = nlohmann::json;

///
/// Shader data
///
using shader_data_t = std::string;

///
/// Generic buffer data.
///
using buffer_data_t = std::vector<unsigned char>;

///
/// Generic image data.
///
struct image_data_t {
    /// Width
    int width;

    /// Height
    int height;

    /// Number of Channels
    int ncomp;

    /// Buffer data for 8-bit images
    std::vector<uint8_t> datab;

    /// Buffer data for float images
    std::vector<float> dataf;
};

///
/// Extensions
///
using extension_t = std::map<std::string, json>;

///
/// Extras
///
using extras_t = json;

// #codegen begin type ---------------------------------------------------------
///
/// No description in schema.
///
struct glTFProperty_t {
    /// No description in schema.
    //extension_t extensions = {};
    extension_t extensions{};
    /// No description in schema.
    extras_t extras = {};
};

///
/// No description in schema.
///
struct glTFChildOfRootProperty_t : glTFProperty_t {
    /// The user-defined name of this object.
    std::string name = "";
};

///
/// Indices of those attributes that deviate from their initialization value.
///
struct accessor_sparse_indices_t : glTFProperty_t {
    /// Values for componentType
    enum struct componentType_t {
        unsigned_byte_t = 5121,
        unsigned_short_t = 5123,
        unsigned_int_t = 5125,
    };

    /// The index of the bufferView with sparse indices. Referenced bufferView
    /// can't have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target. [required]
    int bufferView = -1;
    /// The offset relative to the start of the bufferView in bytes. Must be
    /// aligned.
    int byteOffset = 0;
    /// The indices data type. [required]
    componentType_t componentType = componentType_t::unsigned_byte_t;
};

///
/// Array of size `accessor.sparse.count` times number of components storing the
/// displaced accessor attributes pointed by `accessor.sparse.indices`.
///
struct accessor_sparse_values_t : glTFProperty_t {
    /// The index of the bufferView with sparse values. Referenced bufferView
    /// can't have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target. [required]
    int bufferView = -1;
    /// The offset relative to the start of the bufferView in bytes. Must be
    /// aligned.
    int byteOffset = 0;
};

///
/// Sparse storage of attributes that deviate from their initialization value.
///
struct accessor_sparse_t : glTFProperty_t {
    /// Number of entries stored in the sparse array. [required]
    int count = -1;
    /// Index array of size `count` that points to those accessor attributes
    /// that deviate from their initialization value. Indices must strictly
    /// increase. [required]
    accessor_sparse_indices_t indices = {};
    /// Array of size `count` times number of components, storing the displaced
    /// accessor attributes pointed by `indices`. Substituted values must have
    /// the same `componentType` and number of components as the base accessor.
    /// [required]
    accessor_sparse_values_t values = {};
};

///
/// A typed view into a bufferView.  A bufferView contains raw binary data.  An
/// accessor provides a typed view into a bufferView or a subset of a bufferView
/// similar to how WebGL's `vertexAttribPointer()` defines an attribute in a
/// buffer.
///
struct accessor_t : glTFChildOfRootProperty_t {
    /// Values for componentType
    enum struct componentType_t {
        byte_t = 5120,
        unsigned_byte_t = 5121,
        short_t = 5122,
        unsigned_short_t = 5123,
        unsigned_int_t = 5125,
        float_t = 5126,
    };

    /// Values for type
    enum struct type_t {
        scalar_t = 0,
        vec2_t = 1,
        vec3_t = 2,
        vec4_t = 3,
        mat2_t = 4,
        mat3_t = 5,
        mat4_t = 6,
    };

    /// The index of the bufferView.
    int bufferView = -1;
    /// The offset relative to the start of the bufferView in bytes.
    int byteOffset = 0;
    /// The datatype of components in the attribute. [required]
    componentType_t componentType = componentType_t::byte_t;
    /// The number of attributes referenced by this accessor. [required]
    int count = -1;
    /// Maximum value of each component in this attribute. [required]
    std::vector<float> max = {};
    /// Minimum value of each component in this attribute. [required]
    std::vector<float> min = {};
    /// Specifies whether integer data values should be normalized.
    bool normalized = false;
    /// Sparse storage of attributes that deviate from their initialization
    /// value.
    accessor_sparse_t sparse = {};
    /// Specifies if the attribute is a scalar, vector, or matrix. [required]
    type_t type = type_t::scalar_t;
};

///
/// The index of the node and TRS property that an animation channel targets.
///
struct animation_channel_target_t : glTFProperty_t {
    /// Values for path
    enum struct path_t {
        translation_t = 0,
        rotation_t = 1,
        scale_t = 2,
    };

    /// The index of the node to target. [required]
    int node = -1;
    /// The name of the node's TRS property to modify. [required]
    path_t path = path_t::translation_t;
};

///
/// Targets an animation's sampler at a node's property.
///
struct animation_channel_t : glTFProperty_t {
    /// The index of a sampler in this animation used to compute the value for
    /// the target. [required]
    int sampler = -1;
    /// The index of the node and TRS property to target. [required]
    animation_channel_target_t target = {};
};

///
/// Combines input and output accessors with an interpolation algorithm to
/// define a keyframe graph (but not its target).
///
struct animation_sampler_t : glTFProperty_t {
    /// Values for interpolation
    enum struct interpolation_t {
        linear_t = 0,
        step_t = 1,
    };

    /// The index of an accessor containing keyframe input values, e.g., time.
    /// [required]
    int input = -1;
    /// Interpolation algorithm.
    interpolation_t interpolation = interpolation_t::linear_t;
    /// The index of an accessor, containing keyframe output values. [required]
    int output = -1;
};

///
/// A keyframe animation.
///
struct animation_t : glTFChildOfRootProperty_t {
    /// An array of channels, each of which targets an animation's sampler at a
    /// node's property. Different channels of the same animation can't have
    /// equal targets. [required]
    std::vector<animation_channel_t> channels = {};
    /// An array of samplers that combines input and output accessors with an
    /// interpolation algorithm to define a keyframe graph (but not its target).
    /// [required]
    std::vector<animation_sampler_t> samplers = {};
};

///
/// Metadata about the glTF asset.
///
struct asset_t : glTFProperty_t {
    /// Values for version
    enum struct version_t {
        _2_0_t = 0,
    };

    /// A copyright message suitable for display to credit the content creator.
    std::string copyright = "";
    /// Tool that generated this glTF model.  Useful for debugging.
    std::string generator = "";
    /// The glTF version. [required]
    version_t version = version_t::_2_0_t;
};

///
/// A buffer points to binary geometry, animation, or skins.
///
struct buffer_t : glTFChildOfRootProperty_t {
    /// The length of the buffer in bytes. [required]
    int byteLength = 0;
    /// The uri of the buffer.
    std::string uri = "";

    /// loaded buffer data
    buffer_data_t data;
};

///
/// A view into a buffer generally representing a subset of the buffer.
///
struct bufferView_t : glTFChildOfRootProperty_t {
    /// Values for target
    enum struct target_t {
        array_buffer_t = 34962,
        element_array_buffer_t = 34963,
    };

    /// The index of the buffer. [required]
    int buffer = -1;
    /// The length of the bufferView in bytes. [required]
    int byteLength = -1;
    /// The offset into the buffer in bytes. [required]
    int byteOffset = -1;
    /// The stride, in bytes.
    int byteStride = 0;
    /// The target that the WebGL buffer should be bound to.
    target_t target = target_t::array_buffer_t;
};

///
/// An orthographic camera containing properties to create an orthographic
/// projection matrix.
///
struct camera_orthographic_t : glTFProperty_t {
    /// The floating-point horizontal magnification of the view. [required]
    float xmag = -1;
    /// The floating-point vertical magnification of the view. [required]
    float ymag = -1;
    /// The floating-point distance to the far clipping plane. [required]
    float zfar = -1;
    /// The floating-point distance to the near clipping plane. [required]
    float znear = -1;
};

///
/// A perspective camera containing properties to create a perspective
/// projection matrix.
///
struct camera_perspective_t : glTFProperty_t {
    /// The floating-point aspect ratio of the field of view.
    float aspectRatio = -1;
    /// The floating-point vertical field of view in radians. [required]
    float yfov = -1;
    /// The floating-point distance to the far clipping plane.
    float zfar = -1;
    /// The floating-point distance to the near clipping plane. [required]
    float znear = -1;
};

///
/// A camera's projection.  A node can reference a camera to apply a transform
/// to place the camera in the scene.
///
struct camera_t : glTFChildOfRootProperty_t {
    /// Values for type
    enum struct type_t {
        perspective_t = 0,
        orthographic_t = 1,
    };

    /// An orthographic camera containing properties to create an orthographic
    /// projection matrix.
    camera_orthographic_t orthographic = {};
    /// A perspective camera containing properties to create a perspective
    /// projection matrix.
    camera_perspective_t perspective = {};
    /// Specifies if the camera uses a perspective or orthographic projection.
    /// [required]
    type_t type = type_t::perspective_t;
};

///
/// Image data used to create a texture. Image can be referenced by URI or
/// `bufferView` index. `mimeType` is required in the latter case.
///
struct image_t : glTFChildOfRootProperty_t {
    /// The index of the bufferView that contains the image. Use this instead of
    /// the image's uri property.
    int bufferView = -1;
    /// The image's MIME type.
    std::string mimeType = "";
    /// The uri of the image.
    std::string uri = "";

    /// loaded image data
    image_data_t data;
};

///
/// Reference to a texture.
///
struct textureInfoBase_t {
    /// The index of the texture. [required]
    int index = -1;
    /// The set index of texture's TEXCOORD attribute used for texture
    /// coordinate mapping.
    int texCoord = 0;
};

///
/// No description in schema.
///
struct textureInfo_t : textureInfoBase_t {};

///
/// A texture and its sampler.
///
struct texture_t : glTFChildOfRootProperty_t {
    /// Values for format
    enum struct format_t {
        alpha_t = 6406,
        rgb_t = 6407,
        rgba_t = 6408,
        luminance_t = 6409,
        luminance_alpha_t = 6410,
    };

    /// Values for internalFormat
    enum struct internalFormat_t {
        alpha_t = 6406,
        rgb_t = 6407,
        rgba_t = 6408,
        luminance_t = 6409,
        luminance_alpha_t = 6410,
    };

    /// Values for target
    enum struct target_t {
        texture_2d_t = 3553,
    };

    /// Values for type
    enum struct type_t {
        unsigned_byte_t = 5121,
        unsigned_short_5_6_5_t = 33635,
        unsigned_short_4_4_4_4_t = 32819,
        unsigned_short_5_5_5_1_t = 32820,
    };

    /// The texture's format.
    format_t format = format_t::rgba_t;
    /// The texture's internal format.
    internalFormat_t internalFormat = internalFormat_t::rgba_t;
    /// The index of the sampler used by this texture. [required]
    int sampler = -1;
    /// The index of the image used by this texture. [required]
    int source = -1;
    /// The target that the WebGL texture should be bound to.
    target_t target = target_t::texture_2d_t;
    /// Texel datatype.
    type_t type = type_t::unsigned_byte_t;
};

///
/// No description in schema.
///
struct material_normalTextureInfo_t : textureInfoBase_t {
    /// The scalar multiplier applied to each normal vector of the normal
    /// texture.
    float scale = 1;
};

///
/// No description in schema.
///
struct material_occlusionTextureInfo_t : textureInfoBase_t {
    /// A scalar multiplier controlling the amount of occlusion applied.
    float strength = 1;
};

///
/// A set of parameter values that are used to define the metallic-roughness
/// material model from Physically-Based Rendering (PBR) methodology.
///
struct material_pbrMetallicRoughness_t {
    /// The material's base color factor.
    std::array<float, 4> baseColorFactor = {1, 1, 1, 1};
    /// The base color texture.
    textureInfo_t baseColorTexture = {};
    /// The metalness of the material.
    float metallicFactor = 1;
    /// The metallic-roughness texture.
    textureInfo_t metallicRoughnessTexture = {};
    /// The roughness of the material.
    float roughnessFactor = 1;
};

///
/// Array values. Only one vector will be full at any one time.
///
struct arrayValues_t {
    /// number items
    std::vector<float> items_number;
    /// string items
    std::vector<std::string> items_string;
    /// boolean items
    std::vector<bool> items_boolean;
};
///
/// The material appearance of a primitive.
///
struct material_t : glTFChildOfRootProperty_t {
    /// The emissive color of the material.
    std::array<float, 3> emissiveFactor = {0, 0, 0};
    /// The emissive map texture.
    textureInfo_t emissiveTexture = {};
    /// The normal map texture.
    material_normalTextureInfo_t normalTexture = {};
    /// The occlusion map texture.
    material_occlusionTextureInfo_t occlusionTexture = {};
    /// A set of parameter values that are used to define the metallic-roughness
    /// material model from Physically-Based Rendering (PBR) methodology.
    material_pbrMetallicRoughness_t pbrMetallicRoughness = {};
};

///
/// Geometry to be rendered with the given material.
///
struct mesh_primitive_t : glTFProperty_t {
    /// Values for mode
    enum struct mode_t {
        points_t = 0,
        lines_t = 1,
        line_loop_t = 2,
        line_strip_t = 3,
        triangles_t = 4,
        triangle_strip_t = 5,
        triangle_fan_t = 6,
    };

    /// A dictionary object, where each key corresponds to mesh attribute
    /// semantic and each value is the index of the accessor containing
    /// attribute's data. [required]
    //std::map<std::string, int> attributes = {};
    std::map<std::string, int> attributes{};
    /// The index of the accessor that contains the indices.
    int indices = -1;
    /// The index of the material to apply to this primitive when rendering.
    int material = -1;
    /// The type of primitives to render.
    mode_t mode = mode_t::triangles_t;
    /// An array of Morph Targets, each  Morph Target is a dictionary mapping
    /// attributes (only "POSITION" and "NORMAL" supported) to their deviations
    /// in the Morph Target.
    std::vector<int> targets = {};
};

///
/// A set of primitives to be rendered.  A node can contain one or more meshes.
/// A node's transform places the mesh in the scene.
///
struct mesh_t : glTFChildOfRootProperty_t {
    /// An array of primitives, each defining geometry to be rendered with a
    /// material. [required]
    std::vector<mesh_primitive_t> primitives = {};
    /// Array of weights to be applied to the Morph Targets.
    std::vector<float> weights = {};
};

///
/// A node in the node hierarchy.  When the node contains `skin`, all
/// `mesh.primitives` must contain `JOINT` and `WEIGHT` attributes.  A node can
/// have either a `matrix` or any combination of
/// `translation`/`rotation`/`scale` (TRS) properties. TRS properties are
/// converted to matrices and postmultiplied in the `T * R * S` order to compose
/// the transformation matrix; first the scale is applied to the vertices, then
/// the rotation, and then the translation. If none are provided, the transform
/// is the identity. When a node is targeted for animation (referenced by an
/// animation.channel.target), only TRS properties may be present; `matrix` will
/// not be present.
///
struct node_t : glTFChildOfRootProperty_t {
    /// The index of the camera referenced by this node.
    int camera = -1;
    /// The indices of this node's children.
    std::vector<int> children = {};
    /// A floating-point 4x4 transformation matrix stored in column-major order.
    std::array<float, 16> matrix = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    /// The index of the mesh in this node.
    int mesh = -1;
    /// The node's unit quaternion rotation in the order (x, y, z, w), where w
    /// is the scalar.
    std::array<float, 4> rotation = {0, 0, 0, 1};
    /// The node's non-uniform scale.
    std::array<float, 3> scale = {1, 1, 1};
    /// The index of the skin referenced by this node.
    int skin = -1;
    /// The node's translation.
    std::array<float, 3> translation = {0, 0, 0};
    /// The weights of the instantiated Morph Target. Number of elements must
    /// match number of Morph Targets of used mesh.
    std::vector<float> weights = {};
};

///
/// Texture sampler properties for filtering and wrapping modes.
///
struct sampler_t : glTFChildOfRootProperty_t {
    /// Values for magFilter
    enum struct magFilter_t {
        nearest_t = 9728,
        linear_t = 9729,
    };

    /// Values for minFilter
    enum struct minFilter_t {
        nearest_t = 9728,
        linear_t = 9729,
        nearest_mipmap_nearest_t = 9984,
        linear_mipmap_nearest_t = 9985,
        nearest_mipmap_linear_t = 9986,
        linear_mipmap_linear_t = 9987,
    };

    /// Values for wrapS
    enum struct wrapS_t {
        clamp_to_edge_t = 33071,
        mirrored_repeat_t = 33648,
        repeat_t = 10497,
    };

    /// Values for wrapT
    enum struct wrapT_t {
        clamp_to_edge_t = 33071,
        mirrored_repeat_t = 33648,
        repeat_t = 10497,
    };

    /// Magnification filter.
    magFilter_t magFilter = magFilter_t::linear_t;
    /// Minification filter.
    minFilter_t minFilter = minFilter_t::nearest_mipmap_linear_t;
    /// s wrapping mode.
    wrapS_t wrapS = wrapS_t::repeat_t;
    /// t wrapping mode.
    wrapT_t wrapT = wrapT_t::repeat_t;
};

///
/// The root nodes of a scene.
///
struct scene_t : glTFChildOfRootProperty_t {
    /// The indices of each root node.
    std::vector<int> nodes = {};
};

///
/// Joints and matrices defining a skin.
///
struct skin_t : glTFChildOfRootProperty_t {
    /// The index of the accessor containing the floating-point 4x4 inverse-bind
    /// matrices.  The default is that each matrix is a 4x4 identity matrix,
    /// which implies that inverse-bind matrices were pre-applied.
    int inverseBindMatrices = -1;
    /// Indices of skeleton nodes, used as joints in this skin. [required]
    std::vector<int> joints = {};
    /// The index of the node used as a skeleton root. When undefined, joints
    /// transforms resolve to scene root.
    int skeleton = -1;
};

///
/// The root object for a glTF asset.
///
struct glTF_t : glTFProperty_t {
    /// An array of accessors.
    std::vector<accessor_t> accessors = {};
    /// An array of keyframe animations.
    std::vector<animation_t> animations = {};
    /// Metadata about the glTF asset. [required]
    asset_t asset = {};
    /// An array of bufferViews.
    std::vector<bufferView_t> bufferViews = {};
    /// An array of buffers.
    std::vector<buffer_t> buffers = {};
    /// An array of cameras.
    std::vector<camera_t> cameras = {};
    /// Names of glTF extensions required to properly load this asset.
    std::vector<std::string> extensionsRequired = {};
    /// Names of glTF extensions used somewhere in this asset.
    std::vector<std::string> extensionsUsed = {};
    /// An array of images.
    std::vector<image_t> images = {};
    /// An array of materials.
    std::vector<material_t> materials = {};
    /// An array of meshes.
    std::vector<mesh_t> meshes = {};
    /// An array of nodes.
    std::vector<node_t> nodes = {};
    /// An array of samplers.
    std::vector<sampler_t> samplers = {};
    /// The index of the default scene.
    int scene = -1;
    /// An array of scenes.
    std::vector<scene_t> scenes = {};
    /// An array of skins.
    std::vector<skin_t> skins = {};
    /// An array of textures.
    std::vector<texture_t> textures = {};
};

// #codegen end type -----------------------------------------------------------

///
/// Error when reading/writing gltf.
///
struct gltf_exception : std::exception {
    /// constructor with error message
    gltf_exception(const std::string& errmsg) : _errmsg(errmsg) {}

    /// retieval of error message
    virtual const char* what() const throw() { return _errmsg.c_str(); }

   private:
    std::string _errmsg;
};

///
/// Loads a gltf file from disk
///
/// Parameters:
/// - filename: scene filename
/// - load_bin/load_shaders/load_img: load binary data
/// - skip_missing: do not throw an exception if a file is missing
///
/// Returns:
/// - gltf data loaded
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API glTF_t* load_gltf(const std::string& filename, bool load_bin = true,
    bool load_shaders = true, bool load_img = true, bool skip_missing = false);

///
/// Loads a binary gltf file from disk
///
/// Parameters:
/// - filename: scene filename
/// - other params as above
///
/// Returns:
/// - gltf data loaded
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API glTF_t* load_binary_gltf(const std::string& filename,
    bool load_bin = true, bool load_shaders = true, bool load_img = true,
    bool skip_missing = false);

///
/// Saves a scene to disk
///
/// Parameters:
/// - filename: scene filename
/// - gltf: data to save
/// - save_bin/save_shaders/save_img: save binary data
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void save_gltf(const std::string& filename, const glTF_t* gltf,
    bool save_bin = true, bool save_shaders = true, bool save_images = true);

///
/// Saves a scene to disk
///
/// Parameters:
/// - filename: scene filename
/// - gltf: data to save
/// - save_bin/save_shaders/save_img: save binary data
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void save_binary_gltf(const std::string& filename, const glTF_t* gltf,
    bool save_bin = true, bool save_shaders = true, bool save_images = true);

///
/// Load buffer data.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - skip_missing: do not throw an exception if a file is missing
///
/// Out Parameters:
/// - gltf: data to data
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void load_buffers(
    glTF_t* gltf, const std::string& dirname, bool skip_missing = false);

///
/// Load shaders data.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - skip_missing: do not throw an exception if a file is missing
///
/// Out Parameters:
/// - gltf: data to data
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void load_shaders(
    glTF_t* asset, const std::string& dirname, bool skip_missing = false);

///
/// Loads images.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - skip_missing: do not throw an exception if a file is missing
///
/// Out Parameters:
/// - gltf: data to data
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void load_images(
    glTF_t* asset, const std::string& dirname, bool skip_missing = false);

///
/// Save buffer data.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - gltf: data to save
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void save_buffers(const glTF_t* gltf, const std::string& dirname);

///
/// Save shaders data.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - gltf: data to save
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void save_shaders(const glTF_t* asset, const std::string& dirname);

///
/// Saves images.
///
/// Parameters:
/// - dirname: directory used to resolve path references
/// - gltf: data to save
///
/// Throw:
/// - io_exception: on read/write error
///
YGLTF_API void save_images(const glTF_t* asset, const std::string& dirname);
///
/// A view for gltf array buffers that allows for typed access.
///
struct vec_array_view {
    vec_array_view(const glTF_t* gltf, const accessor_t& accessor);

    int size() const { return _size; }
    int count() const { return _size; }
    int ncomp() const { return _ncomp; }

    std::array<float, 4> operator[](int idx) const;

   private:
    const unsigned char* _data = nullptr;
    int _size = 0;
    int _stride = 0;
    int _ncomp = 0;
    accessor_t::componentType_t _ctype;
    bool _normalize = false;

    static int _num_components(accessor_t::type_t type);
    static int _ctype_size(accessor_t::componentType_t componentType);
};

///
/// A view for gltf element array buffers that allows for typed access.
///
struct element_array_view {
    element_array_view(const glTF_t* gltf, const accessor_t& accessor);

    int size() const { return _size; }

    int operator[](int idx) const;

   private:
    const unsigned char* _data = nullptr;
    int _size = 0;
    int _stride = 0;
    accessor_t::componentType_t _ctype;

    static int _ctype_size(accessor_t::componentType_t componentType);
};

///
/// Computes the local node transform and its inverse.
///
YGLTF_API std::array<float, 16> node_transform(const node_t* node);

// -----------------------------------------------------------------------------
// HIGH-LEVEL, FLATTENED, INTERFACE
// -----------------------------------------------------------------------------

///
/// Camera
///
struct fl_camera {
    std::string name = "";  // name
    std::array<float, 16> xform = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};  // transform
    bool ortho = false;                                   // orthographic
    float aspect = 1;                                     // aspect ratio
    float yfov =
        2 *
        std::atan(0.5f);  // vertical fov (perspective) or size (orthographic)
};

///
/// Primitives
///
struct fl_primitives {
    /// name of the mesh that enclosed it
    std::string name = "";
    /// material reference
    int material = -1;

    /// vertex position
    std::vector<std::array<float, 3>> pos;
    /// vertex normal
    std::vector<std::array<float, 3>> norm;
    /// vertex texcoord
    std::vector<std::array<float, 2>> texcoord;
    /// vertex color
    std::vector<std::array<float, 3>> color;
    /// vertex radius
    std::vector<float> radius;

    /// point elements
    std::vector<int> points;
    /// line elements
    std::vector<std::array<int, 2>> lines;
    /// triangle elements
    std::vector<std::array<int, 3>> triangles;
};

///
/// Material
///
/// glTF 2.0 has two physically-based material models: pbrMetallicRoughness
/// and pbrSpecularGlossiness, the latter as an extension. Here we support both.
/// On load, pbrMetallicRoughness is converted to pbrSpecularGlossiness if
/// the latter is not available (conversion is lossless). On saving,
/// pbrSpecularGlossiness is saved if explicitly always (conversion is
/// lossless). If desired, a lossy conversion from pbrSpecularGlossiness to
/// pbrMetallicRoughness is available as a function call.
///
/// Textures though are not easily convertible since parameters are packed
/// differently in the two specs. For this resaons, textures are not
/// conberted from one method to the other on either load or save.
///
struct fl_material {
    /// name
    std::string name = "";
    /// emission color
    std::array<float, 3> ke = {0, 0, 0};
    /// diffuse color
    std::array<float, 3> kd = {0, 0, 0};
    /// specular color
    std::array<float, 3> ks = {0, 0, 0};
    /// specular roughness
    float rs = 0;
    /// opacity
    float op = 0;
    /// emissive texture reference
    int ke_txt = -1;
    /// diffuse texture reference
    int kd_txt = -1;
    /// specular texture reference
    int ks_txt = -1;
    /// roughness texture reference
    int rs_txt = -1;
};

///
/// Texture
///
struct fl_texture {
    /// name
    std::string name = "";
    /// path
    std::string path = "";
    /// image width
    int width = 0;
    /// image height
    int height = 0;
    /// image number of components
    int ncomp = 0;
    /// 8-bit data
    std::vector<uint8_t> datab;
    /// float data
    std::vector<float> dataf;
};

///
/// Gltf mesh. A collection of primitives with transforms.
///
struct fl_mesh {
    /// name
    std::string name = "";
    /// transform
    std::array<float, 16> xform = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    /// primitives
    std::vector<int> primitives;
};

///
/// Gltf scene
///
struct fl_scene {
    /// name
    std::string name = "";
    /// cameras
    std::vector<int> cameras;
    /// materials
    std::vector<int> materials;
    /// textures
    std::vector<int> textures;
    /// primitives
    std::vector<int> primitives;
    /// meshes
    std::vector<int> meshes;
    /// transforms
    std::vector<int> transforms;
};

///
/// Gltf asset with multiple scenes
///
struct fl_gltf {
    /// default scene (-1 if only one scene is present)
    int default_scene = -1;
    /// cameras
    std::vector<fl_camera*> cameras;
    /// materials
    std::vector<fl_material*> materials;
    /// textures
    std::vector<fl_texture*> textures;
    /// mesh primitives
    std::vector<fl_primitives*> primitives;
    /// meshes
    std::vector<fl_mesh*> meshes;
    /// scenes
    std::vector<fl_scene*> scenes;
};

///
/// Convert a gltf asset to flattened scenes.
///
YGLTF_API fl_gltf* flatten_gltf(const glTF_t* gltf, int scene_idx = -1);

///
/// Convert a flattened gltf to a raw one.
///
YGLTF_API glTF_t* unflatten_gltf(
    const fl_gltf* fl_gltf, const std::string& buffer_uri);
}  // namespace

// -----------------------------------------------------------------------------
// INCLUDE FOR HEADER-ONLY MODE
// -----------------------------------------------------------------------------

#ifdef YGLTF_INLINE
#include "yocto_gltf.cpp"
#endif

#endif
