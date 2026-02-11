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
        ;; TODO: compute with: guix hash -x --serializer=nar <checkout>
        (base32 "0000000000000000000000000000000000000000000000000000"))))
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
