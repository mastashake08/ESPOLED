# Reusable PlatformIO Release Workflow

This repository contains a reusable GitHub Actions workflow for building and releasing PlatformIO firmware projects.

## Usage

### For This Repository

This repository uses the reusable workflow via [release-caller.yml](.github/workflows/release-caller.yml). When you push a version tag (e.g., `v1.0.0`), it automatically builds and releases the firmware.

### For Other Projects

Other repositories can use this reusable workflow in two ways:

#### Option 1: Reference This Repository (Recommended)

Create a workflow file in your project's `.github/workflows/` directory:

```yaml
name: Release Firmware

on:
  push:
    tags:
      - 'v*.*.*'

jobs:
  release:
    uses: mastashake08/ESPOLED/.github/workflows/release.yml@main
    with:
      environment_name: esp32doit-devkit-v1  # Your PlatformIO environment name
      artifact_prefix: MyProject             # Your project name prefix
      release_body: |
        ## My Project Firmware Release
        
        Custom release notes here.
```

#### Option 2: Copy the Workflow

Copy [release.yml](.github/workflows/release.yml) to your project's `.github/workflows/` directory and use it locally.

## Workflow Inputs

| Input | Required | Default | Description |
|-------|----------|---------|-------------|
| `environment_name` | Yes | - | PlatformIO environment name from `platformio.ini` (e.g., `esp32doit-devkit-v1`) |
| `artifact_prefix` | Yes | - | Prefix for release artifact filenames (e.g., `MyProject` → `MyProject-v1.0.0.bin`) |
| `release_body` | No | Basic template | Markdown content for the GitHub release description |
| `python_version` | No | `'3.11'` | Python version to use for PlatformIO |
| `include_elf` | No | `true` | Whether to include the `.elf` file in the release |
| `draft` | No | `false` | Create the release as a draft |
| `prerelease` | No | `false` | Mark the release as a pre-release |

## How It Works

1. **Trigger**: The workflow is triggered by the calling workflow (typically on version tag push)
2. **Build**: Checks out code, installs PlatformIO, and builds the specified environment
3. **Package**: Creates release artifacts with the naming pattern: `{artifact_prefix}-{tag}.bin`
4. **Release**: Creates a GitHub release with the built artifacts and custom release notes

## Examples

### Minimal Configuration

```yaml
jobs:
  release:
    uses: mastashake08/ESPOLED/.github/workflows/release.yml@main
    with:
      environment_name: esp32dev
      artifact_prefix: MyFirmware
```

### Full Configuration

```yaml
jobs:
  release:
    uses: mastashake08/ESPOLED/.github/workflows/release.yml@main
    with:
      environment_name: esp32doit-devkit-v1
      artifact_prefix: SmartDevice
      python_version: '3.12'
      include_elf: false
      draft: false
      prerelease: false
      release_body: |
        ## Smart Device Firmware ${{ github.ref_name }}
        
        ### Changes
        - Feature 1
        - Feature 2
        
        ### Installation
        Flash with: `esptool.py write_flash 0x0 SmartDevice-${{ github.ref_name }}.bin`
```

### Multiple Environments

Build and release for multiple board configurations:

```yaml
jobs:
  release-esp32:
    uses: mastashake08/ESPOLED/.github/workflows/release.yml@main
    with:
      environment_name: esp32dev
      artifact_prefix: MyProject-ESP32
      release_body: |
        ## ESP32 Firmware Release
        
  release-esp32c3:
    uses: mastashake08/ESPOLED/.github/workflows/release.yml@main
    with:
      environment_name: esp32-c3-devkitm-1
      artifact_prefix: MyProject-ESP32C3
      release_body: |
        ## ESP32-C3 Firmware Release
```

## Requirements

Your project must:
- Use PlatformIO with a valid `platformio.ini` configuration
- Have the specified environment name defined in `platformio.ini`
- Have repository write permissions for creating releases

## Permissions

The workflow requires `contents: write` permission to create releases. This is configured in the reusable workflow.

## Versioning

Use semantic versioning tags:
- `v1.0.0` - Major release
- `v1.0.1` - Patch release
- `v1.1.0` - Minor release
- `v2.0.0-beta.1` - Pre-release (set `prerelease: true`)

## Support

For issues or questions about this reusable workflow, please open an issue in the [ESPOLED repository](https://github.com/mastashake08/ESPOLED).
