# Real wood generator

The result of this bachelor's thesis is a Blender add-on for version 4.3.2 which generates 3D wood material.

You can install it by opening Edit > Preferences > Add-ons > Install from disk: plugin.zip.

You use the plugin by going to the Shading tab, then clicking Add > RealWood, and the material will appear among the others.

### RealWood parameters:
- Coordinates: don't change unless you know, what you are doing
- Rotation: rotation of the trunk center
- Scale: scale of the input coordinates
- Noise scale: scale of noise affecting rings, rays, knots
- Noise Variation: choose variantion

Rings
- Rings scale
- Latewood width: prefer the smallest it gets, affects width of darker rings
- Rings Offset: offset from trunk center
- Rings Distorstion: not like noise, it distorts the rings width
- Rings Distorstion Detail Scale: scale of distorsion
- Rings Noise Factor: it adds colored flecks on the rings, based on their color
- Rings Noise Scale
- Earlywood Color: lighter rings color
- Latewood Color: darker rings color

Knots
- Knots Scale
- Knots Variation
- Knots Coverage: how much of the knot area should be colored
- Knot Tightness: how squeezed are the knots in horizontal axis
- Knots Height
- Knots Border: how visible should the black knot border area
- Knots Roughness: can be used for hiding knots or distorting their shape
- Knots Angle Seed: value on which are generated the angle values for knot rotation mask
- Knots Color

Heartwood
- Heartwood Radius: older darker wood width
- Heartwood Color

Pores
- Pores Scale
- Pores Noise Scale
- Pores Width
- Pores Roughness: can be used for hiding pores or distorting their shape
- Pores Color

Rays
- Rays Scale
- Rays Tightness: how squeezed are the rays in horizontal axis
- Rays Height
- Rays Roughness: can be used for hiding rays or distorting their shape
- Rays Color

Flecks
- Flecks Scale
- Flecks Variation: choose variation
- Flecks Roughness: can be used for hiding flecks or distorting their shape
- Flecks Color

Reflections
- Bump Strength: amount of bump
- Bump Pores Strength: amount of pores bump
- Reflection Strength: amount of reflection
- Reflection Rays Strength: amount of rays reflection
- Roughness: roughness of material - 0 behaves like mirror, 1 behaves like gravel

### BSDF parameters:
- Base Color: colored 3D wood texture
- Metallic: don't use for wood (0)
- Roughness: generated based roughness parameter
- IOR: correct IOR for finished wood - 1.55, for raw wood / cellulose - 1.46
- If using subsurface scattering (Weight > 0):
    - Subsurface Weight: amount of subsurface highlight
    - Radius: based on raw wood / cellulose measurements
    - Scale: approximately the right depth of subsurface scattering
    - Anisotropy: subsurface scattering light directionality, aropund 0.5, never 0 or 1
- IOR Level: generated specular reflections based on reflection strength parameter
- Tint: for non-metallic materials always use white
- Anisotropic: amount of anisotropic specular reflection without knots, around 0.5, never 0 or 1
- Anisoptropic Rotation: anisotropic specular reflection rotation based on tangent
- Tangent: direction of anisotropic specular reflection



