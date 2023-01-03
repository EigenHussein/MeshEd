#ifndef CYC_WAVELETS_HH__
#define CYC_WAVELETS_HH__

#include <complex>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

template <typename _T> _T sinc(const _T x) {
    return sin(M_PI * x) / (0.000001 + M_PI * x);
}

typedef std::complex<float> CPX;

#define J CPX(0, 1)

// template <typename output_type, typename input_type>
// template <output_type (*Wavelet)(input_type)>
// static output_type NormalizeWavelet(const input_type &t, input_type a, input_type b) {
// }

typedef float wavelet_dtype;
typedef std::complex<wavelet_dtype> CPX;

// template <typename _input_t, typename _output_t> _output_t raw_wavelet(const _input_t t) {
//     return {t, t};
// }
static float a = 1, b = 0, freq = 1;
CPX mother_wavelet(const float t) {
    return exp(J * 2.f * float(M_PI) * freq * t) * exp(-t * t);
}

template <typename _input_t, typename _output_t, _output_t (*_raw_function)(const _input_t)>
_output_t NormalizeWavelet(_input_t _t, _input_t _a, _input_t _b) {
    return (_input_t(1) / sqrt(_a) * _raw_function((_t - _b) / _a));
}

template <CPX (*ComplexFunc)(const float, const float, const float)> glm::vec3 CPXt_to_xyz(const float t) {
    auto tmp = ComplexFunc(t, a, b);
    return {t, tmp.real(), tmp.imag()};
}

// #define DefinedNormalization()

#endif