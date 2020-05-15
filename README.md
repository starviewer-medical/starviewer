[![Starviewer Logo](http://starviewer.udg.edu/images/starviewer-logo.png)](http://starviewer.udg.edu)

[![Build status](https://ci.appveyor.com/api/projects/status/a83esdyknc0af9od/branch/devel?svg=true)](https://ci.appveyor.com/project/starviewer/starviewer/branch/devel)
[![Build Status](https://travis-ci.org/starviewer-medical/starviewer.svg?branch=devel)](https://travis-ci.org/starviewer-medical/starviewer)

# Starviewer

Starviewer is a cross-platform medical imaging software dedicated to DICOM images produced by medical equipment (MRI, CT, PET, PET-CT, CR, MG,...) fully compliant with the DICOM standard for image communication and image file formats. It can also read many other file formats specified by the MetaIO standard (*.mhd files).

Starviewer is able to receive images transferred by DICOM communication protocol from any PACS or medical imaging modality (STORE SCP - Service Class Provider, STORE SCU - Service Class User, and Query/Retrieve). Starviewer enables navigation and visualization of multimodality and multidimensional images through a complete 2D Viewer which integrates advanced reconstruction techniques such as Thick Slab (including Maximum Intensity Projection (MIP), Minimum Intensity Projection (MinIP) and average projection), fast orthogonal reconstruction, 3D navigation tools such as 3D-Cursor, and basic support for PET-CT image fusion. It also incorporates Multi-Planar Reconstruction (MPR) and 3D Viewer for volume rendering.

## Building Starviewer

Run all.sh

Starviewer build is automated by the scripts placed under `./sdk/scripts/unix`

    sdk/scripts/unix
    ├── all.sh
    │   - Automatically do everything needed to compile.
    ├── configure_system.sh
    │   - Installs system-wide development dependencies and tools.
    ├── generate_prefixfiles.sh
    │   - Generates prefix.sh and prefix.bat files where the environment is saved.
    ├── inspect_environment.sh
    │   - Shows the tools and environment variables effectively used by the scripts.
    ├── sdk_all.sh
    │   - Invokes all SDK related scripts.
    ├── sdk_build.sh
    ├── sdk_download.sh
    │   - Downloads and verifies using hardcoded SHA256 hashes.
    ├── sdk_extract.sh
    ├── starviewer_all.sh
    │   - Invokes all Starviewer related scripts.
    ├── starviewer_build.sh
    ├── starviewer_pack_dpkg.sh
    │   - Generates a .deb package with most dependencies bundled inside.
    └── starviewer_install.sh
        - Generates a multiplatform «portable» folder with everything required to run Starviewer.
        - Those dependencies are automatically detected using ldd.

### Windows

1. Install MSys
2. Open an MSys shell as administrator and run the configure_system.sh script.
3. Reboot
4. Open another MSys shell and run all.sh

Note: in case of weird errors and failures, especially for configure_system.sh expect them; simply open and close the MSYS2 window and retry.

We do not include Microsoft VC libraries in the Starviewer redistribtion package. Doing so conflicts with the GPLv3 termns. If they are not installed in your system download them [here](https://aka.ms/vs/16/release/vc_redist.x64.exe).

## Contribution

Coming soon.

## License

Released under the GPLv3 license. See [LICENSE](https://github.com/starviewer-medical/starviewer/blob/devel/starviewer/LICENSE).
