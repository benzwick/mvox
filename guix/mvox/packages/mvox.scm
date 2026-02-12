(define-module (mvox packages mvox)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix git-download)
  #:use-module (guix build-system cmake)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages compression)
  #:use-module (gnu packages image-processing)
  #:use-module (mvox packages mfem)
  #:use-module (guix utils))

(define-public mvox
  (package
    (name "mvox")
    (version "1.0.0")
    (source
     (origin
       (method git-fetch)
       (uri (git-reference
             (url "https://github.com/benzwick/mvox")
             (commit (string-append "v" version))))
       (file-name (git-file-name name version))
       (snippet
        ;; Remove the Guix channel directory so the source hash does not
        ;; depend on the package definitions (avoids chicken-and-egg when
        ;; tagging releases).
        #~(delete-file-recursively "guix"))
       (sha256
        (base32 "1gka4nlg131dnrya6xzlzjpjd5c1crsgwvgzapls82jfg6pdh0gd"))))
    (build-system cmake-build-system)
    (arguments
     (list
      #:configure-flags
      #~(list (string-append "-DMFEM_DIR=" #$(this-package-input "mfem"))
              "-DBUILD_SHARED_LIBS=ON")
      #:tests? #f))
    (inputs (list mfem insight-toolkit zlib))
    (home-page "https://github.com/benzwick/mvox")
    (synopsis "Mesh voxelizer for finite element meshes")
    (description
     "MVox is a mesh voxelizer that converts MFEM finite element meshes to
voxel images.  It can produce NIfTI and other medical image formats via ITK,
enabling interoperability between finite element simulations and medical
imaging tools such as 3D Slicer.")
    (license license:bsd-3)))
