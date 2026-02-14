# CI/CD Workflows

## Pipeline

```
C/C++ CI (c-cpp.yml)
  ├── build (matrix: ubuntu-22.04, ubuntu-latest)
  └── docs (documentation.yml) — called after build succeeds
```

1. **C/C++ CI** builds and tests MVox, uploads example artifacts (NRRD inputs + VTK meshes)
2. **Documentation** is called as a reusable workflow after build succeeds (not on PRs) — downloads artifacts, captures screenshots with 3D Slicer, builds Sphinx docs, deploys to GitHub Pages

To manually trigger a docs rebuild, dispatch the C/C++ CI workflow.

## Workflows

| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `c-cpp.yml` | push, PR, weekly schedule, manual | Build and test MVox, call docs workflow |
| `documentation.yml` | called by `c-cpp.yml` | Build and deploy documentation |
