# CI/CD Workflows

## Pipeline

```
C/C++ CI (c-cpp.yml)  →  Documentation (docs.yml)
```

1. **C/C++ CI** builds and tests MVox, uploads example artifacts (NRRD inputs + VTK meshes)
2. **Documentation** runs automatically when CI succeeds on `main` — downloads artifacts, captures screenshots with 3D Slicer, builds Sphinx docs, deploys to GitHub Pages

To manually trigger a docs rebuild, dispatch the C/C++ CI workflow.

## Workflows

| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `c-cpp.yml` | push, PR, weekly schedule, manual | Build and test MVox |
| `documentation.yml` | C/C++ CI success on `main` | Build and deploy documentation |
