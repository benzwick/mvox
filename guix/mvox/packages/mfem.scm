(define-module (mvox packages mfem)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix download)
  #:use-module (guix build-system cmake)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages compression))

(define-public mfem
  (package
    (name "mfem")
    (version "4.5.2")
    (source
     (origin
       (method url-fetch)
       (uri (string-append
             "https://github.com/mfem/mfem/archive/refs/tags/v"
             version ".tar.gz"))
       (sha256
        (base32 "lccfdm05lr1yqjlq44ccpx9yxpqyv11ly7c70c8k641r7l1pgmj0"))))
    (build-system cmake-build-system)
    (arguments
     (list
      #:configure-flags
      #~(list "-DBUILD_SHARED_LIBS=YES"
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
