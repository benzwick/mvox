"""Capture screenshots of MVox inputs and outputs using 3D Slicer.

Run with: Slicer --python-script scripts/capture_screenshots.py --exit
"""

import os
import slicer


def capture_view(filename, widget):
    """Capture a screenshot from a Slicer view widget."""
    image = widget.grab()
    image.save(filename)
    print(f"Saved {filename}")


def capture_slice_views(output_dir, prefix):
    """Capture axial, sagittal, and coronal slice views."""
    layout_manager = slicer.app.layoutManager()
    for color, name in [("Red", "axial"), ("Yellow", "sagittal"), ("Green", "coronal")]:
        widget = layout_manager.sliceWidget(color)
        widget.sliceController().fitSliceToBackground()
        slicer.app.processEvents()
        path = os.path.join(output_dir, f"{prefix}_{name}.png")
        capture_view(path, widget)


def capture_3d_view(output_dir, prefix):
    """Capture the 3D view."""
    layout_manager = slicer.app.layoutManager()
    view_widget = layout_manager.threeDWidget(0)
    view_node = view_widget.mrmlViewNode()

    # Reset the 3D view to a standard orientation and fit
    view_node.SetBoxVisible(False)
    view_node.SetAxisLabelsVisible(False)

    renderer = view_widget.threeDView().renderWindow().GetRenderers().GetFirstRenderer()
    renderer.ResetCamera()
    slicer.app.processEvents()

    path = os.path.join(output_dir, f"{prefix}_3d.png")
    capture_view(path, view_widget)


def boxmesh_example(output_dir):
    """Capture screenshots for the boxmesh example."""
    # Set four-up layout (3 slice views + 3D)
    slicer.app.layoutManager().setLayout(
        slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView
    )

    # Load input volume
    volume_path = os.path.join("examples", "A1_grayT1-1mm_resample.nrrd")
    if not os.path.exists(volume_path):
        print(f"Warning: {volume_path} not found, skipping boxmesh input screenshots")
    else:
        volume_node = slicer.util.loadVolume(volume_path)
        slicer.util.setSliceViewerLayers(background=volume_node)
        slicer.app.processEvents()
        capture_slice_views(output_dir, "boxmesh_input")
        slicer.mrmlScene.RemoveNode(volume_node)

    # Load output mesh
    mesh_path = os.path.join("examples", "boxmesh.vtk")
    if not os.path.exists(mesh_path):
        print(f"Warning: {mesh_path} not found, skipping boxmesh mesh screenshots")
    else:
        model_node = slicer.util.loadModel(mesh_path)
        model_node.GetDisplayNode().SetEdgeVisibility(True)
        slicer.app.processEvents()
        capture_3d_view(output_dir, "boxmesh_mesh")
        slicer.mrmlScene.RemoveNode(model_node)


def brain_tensors_example(output_dir):
    """Capture screenshots for the brain tensors example."""
    slicer.app.layoutManager().setLayout(
        slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView
    )

    # Load label map
    labelmap_path = os.path.join("examples", "labelmap.nrrd")
    if not os.path.exists(labelmap_path):
        print(f"Warning: {labelmap_path} not found, skipping brain labelmap screenshots")
    else:
        labelmap_node = slicer.util.loadLabelVolume(labelmap_path)
        slicer.util.setSliceViewerLayers(label=labelmap_node)
        slicer.app.processEvents()
        capture_slice_views(output_dir, "brain_labelmap")
        slicer.mrmlScene.RemoveNode(labelmap_node)

    # Load output mesh
    mesh_path = os.path.join("examples", "brain.vtk")
    if not os.path.exists(mesh_path):
        print(f"Warning: {mesh_path} not found, skipping brain mesh screenshots")
    else:
        model_node = slicer.util.loadModel(mesh_path)
        model_node.GetDisplayNode().SetEdgeVisibility(True)
        slicer.app.processEvents()
        capture_3d_view(output_dir, "brain_mesh")
        slicer.mrmlScene.RemoveNode(model_node)


def main():
    output_dir = "screenshots"
    os.makedirs(output_dir, exist_ok=True)

    boxmesh_example(output_dir)

    # Clear scene between examples
    slicer.mrmlScene.Clear(False)

    brain_tensors_example(output_dir)

    print(f"Screenshots saved to {output_dir}/")


main()
