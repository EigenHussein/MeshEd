#ifndef FOURIER_UTILS_HH
#define FOURIER_UTILS_HH

#include <fftw3.h>
#include <iostream>

template <typename _FP> class FFTClass {

  private:
    int len;
    fftw_complex *input_double;
    fftw_complex *output_double;

    fftwf_complex *input_float;
    fftwf_complex *output_float;

    fftw_plan fftp_d;
    fftwf_plan fftp_f;

    bool Initialized;

  public:
    FFTClass(int length = 1024) {
        len = length;
        if (std::is_same<_FP, double>::value) {
            input_double = fftw_alloc_complex(length);
            output_double = fftw_alloc_complex(length);

            fftp_d = fftw_plan_dft_1d(length, input_double, output_double, FFTW_FORWARD, FFTW_ESTIMATE);

            input_float = output_float = 0;
            Initialized = true;
        } else if (std::is_same<_FP, float>::value) {
            input_float = fftwf_alloc_complex(length);
            output_float = fftwf_alloc_complex(length);

            fftp_f = fftwf_plan_dft_1d(length, input_float, output_float, FFTW_FORWARD, FFTW_ESTIMATE);

            input_double = output_double = 0;
            Initialized = true;
        } else {
            std::cout << "Only floats and doubles" << std::endl;
        }
    }

    void Perform() {
        if (std::is_same<_FP, double>::value) {
            fftw_execute(fftp_d);
        } else if (std::is_same<_FP, float>::value) {
            fftwf_execute(fftp_f);
        }
    }
    void *getInputBuffer() const {
        if (!Initialized)
            return 0;
        if (std::is_same<_FP, double>::value) {
            return input_double;
        } else {
            return input_float;
        }
    }
    void *getOutputBuffer() const {
        if (!Initialized)
            return 0;
        if (std::is_same<_FP, double>::value) {
            return output_double;
        } else {
            return output_float;
        }
    }

    ~FFTClass() {
        if (Initialized) {
            if (std::is_same<_FP, double>::value) {
                fftw_destroy_plan(fftp_d);
                free(input_double);
                free(output_double);
            } else if (std::is_same<_FP, float>::value) {
                fftwf_destroy_plan(fftp_f);
                free(input_float);
                free(output_float);
            }
        }
    }
};

#endif