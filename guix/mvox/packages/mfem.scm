(define-module (mvox packages mfem)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix git-download)
  #:use-module (guix build-system cmake)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages compression))

(define-public mfem
  (package
    (name "mfem")
    (version "4.5.2")
    (source
     (origin
       (method git-fetch)
       (uri (git-reference
             (url "https://github.com/mfem/mfem")
             (commit (string-append "v" version))))
       (file-name (git-file-name name version))
       (sha256
        (base32 "1hs2n278ggmq8gpvgsfgslh6lmx1mgdwywk31alfvdpj97d5nr5m"))))
    (build-system cmake-build-system)
    (arguments
     (list
      #:configure-flags
      #~(list "-DCMAKE_CXX_FLAGS=-include cstdint"
              "-DBUILD_SHARED_LIBS=YES"
              "-DMFEM_USE_ZLIB=YES"
              "-DMFEM_USE_EXCEPTIONS=YES"
              "-DMFEM_ENABLE_TESTING=NO"
              "-DMFEM_ENABLE_EXAMPLES=NO"
              "-DMFEM_ENABLE_MINIAPPS=NO")
      #:tests? #f))
    (inputs (list zlib))
    (home-page "https://mfem.org")
    (synopsis "Finite element methods library")
    (description
     "MFEM is a modular parallel C++ library for finite element methods.
Its goal is to enable high-performance scalable finite element
discretization research and application development on a wide variety
of platforms, ranging from laptops to supercomputers.")
    (license license:bsd-3)))
