import os
import shutil

import patch


def prepare_netgen():
    """
    Prepare sources for Netgen.
    """
    # Delete source directories if they exist
    if os.path.exists('src/Netgen'):
        shutil.rmtree('src/Netgen')

    # Copy sources
    shutil.copytree('external/Netgen/libsrc', 'src/Netgen/libsrc')
    shutil.copytree('external/Netgen/nglib', 'src/Netgen/nglib')

    # Patch Netgen sources for SALOME
    pset = patch.fromfile('external/NETGENPlugin/src/NETGEN/netgen62ForSalome.patch')
    success = pset.apply(strip=1, root='src/Netgen')
    if not success:
        raise RuntimeError('Failed to apply Netgen patch.')

    # Patch occgenmesh for OCCT 7.6
    pset = patch.fromfile('patch/occgenmesh_OCCT76.patch')
    success = pset.apply(strip=0, root='src/Netgen')
    if not success:
        raise RuntimeError('Failed to apply occgenmesh for OCCT 7.6 patch.')

    # Copy Netgen cmake files into source directory
    shutil.copytree('cmake/Netgen', 'src/Netgen', dirs_exist_ok=True)

    # Copy netgen_version.hpp
    # Obtained from distribution at https://anaconda.org/ngsolve/netgen
    shutil.copyfile('extra/Netgen/netgen_version.hpp',
                    'src/Netgen/libsrc/include/netgen_version.hpp')

    # Patch for OCCT 7.8.1
    pset = patch.fromfile('patch/netgen_occt781.patch')
    success = pset.apply(strip=0, root='src/Netgen')
    if not success:
        raise RuntimeError('Failed to apply occt7.8.1 patch for Netgen.')

def prepare_kernel():
    """
    Prepare sources for Kernel.
    """
    # Delete source directories if they exist
    if os.path.exists('src/Kernel'):
        shutil.rmtree('src/Kernel')

    # Copy sources
    shutil.copytree('external/Kernel/src/Basics',
                    'src/Kernel/src/Basics')
    shutil.copytree('external/Kernel/src/SALOMELocalTrace',
                    'src/Kernel/src/SALOMELocalTrace')
    shutil.copytree('external/Kernel/src/Utils',
                    'src/Kernel/src/Utils')

    # Copy and overwrite the Kernel CMakeLists.txt file
    target = os.path.join('src/Kernel', 'CMakeLists.txt')
    shutil.copyfile('cmake/Kernel/CMakeLists.txt', target)

    # Patch sources
    pset = patch.fromfile('patch/Kernel.patch')
    success = pset.apply(strip=0, root='src/Kernel')
    if not success:
        raise RuntimeError('Failed to apply Kernel patch.')

    # Patch sources
    pset = patch.fromfile('patch/Kernel_occt781.patch')
    success = pset.apply(strip=0, root='src/Kernel')
    if not success:
        raise RuntimeError('Failed to apply occt7.8.1 patch for Kernel.')

    # Patch sources
    pset = patch.fromfile('patch/kernel_win_cmake.patch')
    success = pset.apply(strip=0, root='src/Kernel')
    if not success:
        raise RuntimeError('Failed to apply kernel_win_cmake.patch patch for Kernel.')


def prepare_geom():
    """
    Prepare sources for Geom.
    """
    # Delete source directories if they exist
    if os.path.exists('src/Geom'):
        shutil.rmtree('src/Geom')

    # Copy sources
    shutil.copytree('external/Geom/src/GEOMUtils',
                    'src/Geom/src/GEOMUtils')

    # Copy and overwrite the Geom CMakeLists.txt file
    target = os.path.join('src/Geom', 'CMakeLists.txt')
    shutil.copyfile('cmake/Geom/CMakeLists.txt', target)

    # Patch sources
    pset = patch.fromfile('patch/GEOMUtils.patch')
    success = pset.apply(strip=0, root='src/Geom')
    if not success:
        raise RuntimeError('Failed to apply GEOMUtils patch.')


def prepare_smesh():
    """
    Prepare sources for SMESH.
    """
    # Delete source directories if they exist
    if os.path.exists('src/SMESH'):
        shutil.rmtree('src/SMESH')

    # Copy sources
    shutil.copytree('external/SMESH/src',
                    'src/SMESH/src')

    # Copy and overwrite the SMESH CMakeLists.txt file
    target = os.path.join('src/SMESH', 'CMakeLists.txt')
    shutil.copyfile('cmake/SMESH/CMakeLists.txt', target)

    # Patch sources
    pset = patch.fromfile('patch/mefisto.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply mefisto patch.')

    pset = patch.fromfile('patch/SMESH_ControlPnt.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_ControlPnt patch.')

    pset = patch.fromfile('patch/SMESH_Controls.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_Controls patch.')

    pset = patch.fromfile('patch/SMESH_Mesh.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_Mesh patch.')

    pset = patch.fromfile('patch/SMESH_MeshAlgos.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_MeshAlgos patch.')

    pset = patch.fromfile('patch/SMESH_Slot.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_Slot patch.')

    pset = patch.fromfile('patch/SMESH_SMDS.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply SMESH_SMDS patch.')

    pset = patch.fromfile('patch/StdMeshers_Adaptive1D.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply StdMeshers_Adaptive1D patch.')

    pset = patch.fromfile('patch/StdMeshers_Projection_2D.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply StdMeshers_Projection_2D patch.')

    pset = patch.fromfile('patch/StdMeshers_ViscousLayers.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply StdMeshers_ViscousLayers patch.')

    # Copy MeshVSLink sources
    shutil.copytree('extra/MeshVSLink',
                    'src/SMESH/src/MeshVSLink', dirs_exist_ok=True)

    # Copy MEFISTO2 trte.c source
    shutil.copyfile('extra/MEFISTO2/trte.c', 'src/SMESH/src/MEFISTO2/trte.c')

    pset = patch.fromfile('patch/SMESH_occt781.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply occt7.8.1 patch for SMESH.')

    pset = patch.fromfile('patch/vtk_9_4.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply vtk_9_4 patch for SMESH.')    

    pset = patch.fromfile('patch/boost_1_89.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply boost_1_89.patch for SMESH.')

    pset = patch.fromfile('patch/smesh_tkexpr.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply smesh_tkexpr.patch for SMESH.')

    pset = patch.fromfile('patch/smesh_win.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply smesh_win.patch for SMESH.')


def prepare_netgen_plugin():
    """
    Prepare sources for NETGENPlugin
    """
    # Delete source directories if they exist
    if os.path.exists('src/SMESH/src/NETGENPlugin'):
        shutil.rmtree('src/SMESH/src/NETGENPlugin')

    # Copy sources
    shutil.copytree('external/NETGENPlugin/src',
                    'src/SMESH/src/NETGENPlugin/src')

    # Patch sources
    pset = patch.fromfile('patch/NETGENPlugin_Mesher.patch')
    success = pset.apply(strip=0, root='src/SMESH/src/NETGENPlugin')
    if not success:
        raise RuntimeError('Failed to apply NETGENPlugin_Mesher patch.')

    # Patch sources
    pset = patch.fromfile('patch/NETGENPlugin_occt781.patch')
    success = pset.apply(strip=0, root='src/SMESH')
    if not success:
        raise RuntimeError('Failed to apply occt7.8.1 patch for SMESH.')


if __name__ == '__main__':
    prepare_netgen()
    prepare_kernel()
    prepare_geom()
    prepare_smesh()
    prepare_netgen_plugin()
