uniform vec3 base_color;
uniform vec3 sec_color;
uniform vec3 gap_color;

uniform float scale;
uniform float black_stain_scale;
uniform float white_stain_scale;

uniform float tile_size;
uniform float randomness;

varying vec2 vUv;

struct HexTile{
    vec2 _vec;
    float _uniform;
    float _random;
};

struct HexGrid{
    vec3 _colorUni;
    vec3 _colorRnd;
    float _maskUni;
};

vec2 snap(vec2 uv, vec2 increment) {
    return floor(uv / increment) * increment;
}

float whiteNoise4D(vec4 p4) {
    float n = dot(p4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(n) * 43758.5453123);
}

HexTile rowOfHexagons(vec2 input_div, vec2 input_vec, vec2 scale, float tile_size, float randomness) {
    vec2 r_ms = vec2(mod(scale, input_vec) - input_div);
    float r_nd = dot(normalize(input_vec), abs(r_ms));
    float r_am = max(abs(r_ms).x, r_nd);
    float hex = (r_am < (tile_size/2.0)) ? 1.0 : 0.0;

    vec2 snapped = snap(scale, input_vec);
    float white_noise = whiteNoise4D(vec4(snapped, 1234.56, randomness));
    
    return HexTile(vec2(hex) * r_ms , hex, white_noise * hex);
}

HexGrid hexagonGrid(vec2 Uv, float scale, float tile_size, float randomness){
    vec2 scaled_uv = Uv * scale;
    vec2 input_vec = vec2(1.0,sqrt(3.0));
    vec2 input_div = input_vec/2.0;
    vec2 translated_scale = input_div + scaled_uv;

    HexTile row_even = rowOfHexagons(input_div, input_vec, scaled_uv, tile_size, randomness);
    HexTile row_odd = rowOfHexagons(input_div, input_vec, translated_scale, tile_size, randomness);

    vec3 color = vec3(row_even._vec + row_odd._vec, 0.0);
    float border_uni = row_even._uniform + row_odd._uniform;
    float border_rnd = row_even._random + row_odd._random;
    vec3 rnd_field = color + vec3(border_rnd);

    return HexGrid(color, rnd_field, border_uni);
}

// random 2D point in cell
vec2 hash22(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

// random in 0-1 
vec2 hash22_voronoi(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453123); 
}

// Blender "Smooth Min" func
float blender_smooth_min(float a, float b, float c) {
    if (c <= 0.0) return min(a, b);
    float h = max(c - abs(a - b), 0.0) / c;
    return min(a, b) - h * h * h * c * (1.0 / 6.0);
}

// Blender voronoi smooth f1 2D euklidean dist
void node_tex_voronoi_smooth_f1_2d(
    vec3 coord, 
    float scale, 
    float detail, 
    float roughness, 
    float lacunarity, 
    float smoothness, 
    float randomness, 
    float normalize_val, 
    out float outDistance, 
    out vec4 outColor, 
    out vec3 outPosition, 
    out float outW, 
    out float outRadius
) {
    float safe_detail = clamp(detail, 0.0, 15.0);
    float safe_roughness = clamp(roughness, 0.0, 1.0);
    float safe_smoothness = clamp(smoothness / 2.0, 0.0, 0.5); 
    float safe_randomness = clamp(randomness, 0.0, 1.0);
    bool do_normalize = (normalize_val != 0.0);

    vec2 p = coord.xy * scale;

    float f = 0.0;
    float amp = 1.0;
    float max_amp = 0.0;
    int octaves = int(clamp(ceil(safe_detail), 1.0, 16.0));

    for (int i = 0; i < 16; i++) {
        if (i >= octaves) break;

        float current_amp = amp;
        if (i == octaves - 1 && fract(safe_detail) != 0.0) {
            current_amp *= fract(safe_detail);
        }

        vec2 cell = floor(p);
        vec2 fract_pos = p - cell;
        float min_dist = 8.0; 

        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec2 neighbor = vec2(float(x), float(y));
                
                vec2 rand_pos = hash22_voronoi(cell + neighbor);
                vec2 point = mix(vec2(0.5), rand_pos, safe_randomness);
                
                vec2 diff = neighbor + point - fract_pos;
                float dist = length(diff);
                
                min_dist = blender_smooth_min(min_dist, dist, safe_smoothness);
            }
        }

        f += min_dist * current_amp;
        max_amp += current_amp;

        amp *= safe_roughness;
        p *= lacunarity;
    }

    if (do_normalize) {
        float max_distance = 0.5 + 0.5 * safe_randomness; 
        outDistance = f / (max_amp * max_distance);
    } else {
        outDistance = f;
    }

    outColor = vec4(outDistance, outDistance, outDistance, 1.0);
    outPosition = vec3(0.0);
    outW = 0.0;
    outRadius = 0.0;
}

uint hash_uint2(uint x, uint y) {
    uint hash = x + y * 374761393u;
    hash = (hash ^ 61u) ^ (hash >> 16u);
    hash *= 9u;
    hash = hash ^ (hash >> 4u);
    hash *= 668265261u;
    hash = hash ^ (hash >> 15u);
    return hash;
}

float negate_if(float val, uint condition) {
    return (condition != 0u) ? -val : val;
}

float noise_grad(uint hash, float x, float y) {
    uint h = hash & 7u;
    float u = h < 4u ? x : y;
    float v = 2.0 * (h < 4u ? y : x);
    return negate_if(u, h & 1u) + negate_if(v, h & 2u);
}

vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float snoise(vec2 p) {
    vec2 pi = floor(p);
    vec2 pf = p - pi;
    vec2 w = fade(pf);

    uint X = uint(int(pi.x));
    uint Y = uint(int(pi.y));

    uint h00 = hash_uint2(X, Y);
    uint h10 = hash_uint2(X + 1u, Y);
    uint h01 = hash_uint2(X, Y + 1u);
    uint h11 = hash_uint2(X + 1u, Y + 1u);

    float c00 = noise_grad(h00, pf.x, pf.y);
    float c10 = noise_grad(h10, pf.x - 1.0, pf.y);
    float c01 = noise_grad(h01, pf.x, pf.y - 1.0);
    float c11 = noise_grad(h11, pf.x - 1.0, pf.y - 1.0);

    float x1 = mix(c00, c10, w.x);
    float x2 = mix(c01, c11, w.x);
    return mix(x1, x2, w.y) * 2.9;
}

// Blender helper func for offseting
vec2 random_vec2_offset(float seed) {
    return vec2(100.5 + seed * 100.0, 233.7 + seed * 100.0);
}

// Blender fBM noise
float noise_fbm(vec2 p, float detail, float roughness, float lacunarity, float offset, float gain, bool normalize) {
    float f = 0.0;
    float amp = 1.0;
    float maxAmp = 0.0;
    int octaves = int(clamp(ceil(detail), 1.0, 16.0));

    for (int i = 0; i < 16; i++) {
        if (i >= octaves) break;

        float currentAmp = amp;
        if (i == octaves - 1 && fract(detail) != 0.0) {
            currentAmp *= fract(detail);
        }

        f += snoise(p) * currentAmp;
        maxAmp += currentAmp;
        
        amp *= roughness;
        p *= lacunarity;
    }

    if (normalize) {
        return clamp((f / maxAmp) * 0.5 + 0.5, 0.0, 1.0);
    } else {
        return f / maxAmp; 
    }
}

#define NOISE_FRACTAL_DISTORTED_2D(NOISE_TYPE) \
  if (distortion != 0.0f) { \
    p += vec2(snoise(p + random_vec2_offset(0.0f)) * distortion, \
                snoise(p + random_vec2_offset(1.0f)) * distortion); \
  } \
  value = NOISE_TYPE(p, detail, roughness, lacunarity, offset, gain, normalize != 0.0f); \
  if (compute_color != 0.0f) { \
    color = vec4(value, \
                   NOISE_TYPE(p + random_vec2_offset(2.0f), \
                              detail, \
                              roughness, \
                              lacunarity, \
                              offset, \
                              gain, \
                              normalize != 0.0f), \
                   NOISE_TYPE(p + random_vec2_offset(3.0f), \
                              detail, \
                              roughness, \
                              lacunarity, \
                              offset, \
                              gain, \
                              normalize != 0.0f), \
                   1.0f); \
  }

void node_noise_tex_fbm_2d(vec3 co,
                           float scale,
                           float detail,
                           float roughness,
                           float lacunarity,
                           float offset,
                           float gain,
                           float distortion,
                           float normalize,
                           float compute_color,
                           out float value,
                           out vec4 color)
{
  detail = clamp(detail, 0.0f, 15.0f);
  roughness = max(roughness, 0.0f);

  vec2 p = co.xy * scale;

  NOISE_FRACTAL_DISTORTED_2D(noise_fbm)
}

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

float calc_wave_2d(
    vec2 p,
    float distortion,
    float detail,
    float detail_scale,
    float detail_roughness,
    float phase,
    int wave_type,
    int dir,
    int wave_profile
) {
    p = (p + 0.000001) * 0.999999;

    float n = 0.0;

    if (wave_type == 0) { /* type bands */
        if (dir == 0) { /* X axis */
            n = p.x * 20.0;
        }
        else if (dir == 1) { /* Y axis */
            n = p.y * 20.0;
        }
        else { /* Diagonal axis */
            n = (p.x + p.y) * 10.0;
        }
    }
    else { /* type rings */
        vec2 rp = p;
        if (dir == 0) { /* X axis */
            rp *= vec2(0.0, 1.0);
        }
        else if (dir == 1) { /* Y axis */
            rp *= vec2(1.0, 0.0);
        }
        n = length(rp) * 20.0;
    }

    n += phase;

    if (distortion != 0.0) {
        float fbm_val = noise_fbm(p * detail_scale, detail, detail_roughness, 2.0, 0.0, 0.0, true);
        n += distortion * (fbm_val * 2.0 - 1.0);
    }

    if (wave_profile == 0) { /* profile sin */
        return 0.5 + 0.5 * sin(n - M_PI_2);
    }
    else if (wave_profile == 1) { /* profile saw */
        n /= 2.0 * M_PI;
        return n - floor(n);
    }
    else { /* profile tri */
        n /= 2.0 * M_PI;
        return abs(n - floor(n + 0.5)) * 2.0;
    }
}

void node_tex_wave_2d(
    vec2 co,
    int wave_type,
    int dir,
    int wave_profile,
    float scale,
    float distortion,
    float detail,
    float detail_scale,
    float detail_roughness,
    float phase,
    out float fac,    
    out vec4 color    
) {
    float f = calc_wave_2d(
        co * scale,
        distortion,
        detail,
        detail_scale,
        detail_roughness,
        phase,
        wave_type,
        dir,
        wave_profile
    );

    fac = f;
    color = vec4(f, f, f, 1.0);
}

float color_ramp_linear(float value, float black_pos, float white_pos) {
    if (black_pos == white_pos) {
        return step(black_pos, value);
    }
    return clamp((value - black_pos) / (white_pos - black_pos), 0.0, 1.0);
}

void main() {  
    HexGrid hex_grid = hexagonGrid(vUv, scale, tile_size, randomness);

    float value;   
    vec4 color; 
    node_noise_tex_fbm_2d(
        hex_grid._colorRnd,     // UV
        1.5,                    // scale
        15.0,                   // detail
        0.46,                   // roughness
        2.0,                    // lacunarity
        0.0,                    // offset (ignore)
        0.0,                    // gain (ignore)
        0.1,                    // distortion
        1.0,                    // normalize (1.0 = true)
        1.0,                    // compute_color (0.0 off, otherwise on)
        value,                  // value
        color                   // color
    );

    float wave_val;
    vec4 wave_color;

    node_tex_wave_2d(
        vec2(value),     // UV
        0,             // wave_type (Bands)
        0,             // direction (X)
        0,             // profile (Sine)
        0.4,           // scale
        0.0,           // distortion
        15.0,           // detail
        2.7,           // detail_scale
        0.555,           // detail_roughness
        8.3,           // phase_offset
        wave_val,      // out value
        wave_color     // out color
    );

    float wave_range = (wave_val - 0.1) / 0.4;

    vec3 white = vec3(1.0);
    vec3 black = vec3(0.0);

    vec3 colored1 = mix(sec_color, white, wave_range); 
    vec3 colored2 = mix(colored1, base_color, wave_val);

    float outDistance;
    vec4 outColor;
    vec3 outPosition;
    float outW;
    float outRadius;

    node_tex_voronoi_smooth_f1_2d(
        vec3(vUv,0.0),      // vec3 coord, 
        black_stain_scale,// float scale, 
        5.1,// float detail, 
        0.82,// float roughness, 
        2.0,// float lacunarity, 
        1.0,// float smoothness, 
        1.0,// float randomness, 
        1.0,// float normalize_val, 
        outDistance,// out float outDistance, 
        outColor,// out vec4 outColor, 
        outPosition,// out vec3 outPosition, 
        outW,// out float outW, 
        outRadius// out float outRadius
    );

    float color_factor = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    vec3 ramped = mix(white, black, step(0.205, color_factor));
    vec3 ramped2 = vec3(color_ramp_linear(color_factor, 0.949, 0.0));

    node_tex_voronoi_smooth_f1_2d(
        vec3(vUv,0.0),//     vec3 coord, 
        white_stain_scale,// float scale, 
        13.9,// float detail, 
        0.203,// float roughness, 
        2.0,// float lacunarity, 
        1.0,// float smoothness, 
        1.0,// float randomness, 
        1.0,// float normalize_val, 
        outDistance,// out float outDistance, 
        outColor,// out vec4 outColor, 
        outPosition,// out vec3 outPosition, 
        outW,// out float outW, 
        outRadius// out float outRadius
    );

    vec3 ramped3 = mix(white, black, clamp((outDistance - 0.0) / (0.150 - 0.0), 0.0, 1.0));
    vec3 ramped4 = vec3(color_ramp_linear(color_factor, 0.453, 0.105));

    vec3 add_ramps = ramped + ramped3;
    vec3 colored3 = mix(colored2, vec3(0.835), ramped4);
    vec3 colored4 = mix(colored3, vec3(1.0) - ramped, add_ramps * hex_grid._maskUni);
    vec3 colored5 = mix(colored4, gap_color, 1.0 - hex_grid._maskUni);

    node_noise_tex_fbm_2d(
        vec3(vUv,0.0),          // UV)
        15.0,                    // scale
        15.0,                   // detail
        0.86,                  // roughness
        2.0,                    // lacunarity
        0.0,                    // offset (ignore)
        0.0,                    // gain (ignore)
        10.8,                   // distortion
        1.0,                    // normalize (1.0 = true)
        1.0,                    // compute_color (0.0 = dont)
        value,                  //  value
        color                   // color
    );

    vec3 finalColor = vec3(mix(colored5, colored5 * vec3(value), 1.0 - hex_grid._maskUni));

    gl_FragColor = vec4(finalColor, 1.0);
}