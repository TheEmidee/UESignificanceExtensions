# SignificanceExtensions

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.3%2B-blue.svg)](https://unrealengine.com/)
[![Version](https://img.shields.io/badge/version-1.0.0-green.svg)](https://github.com/yourusername/yourplugin/releases)

## Overview

**SignificanceExtensions** contains components to help use the SignificanceManager of the engine

## Installation

You can download these sources directly and put them in your project's `Plugins` folder
Or you can add the plugin as a submodule `git submodule add git@github.com:FishingCactus/UESignificanceExtensions Plugins/SignificanceExtensions`

## Dependencies

* SignificanceManager

## Quick Start

### Basic Setup

1. **Enable the Plugin**
   - Navigate to Edit → Plugins
   - Search for "SignificanceExtensions" and check the enabled box
   - Restart the editor

## Documentation

### Core Classes

  - `USEControllerSignificanceUpdaterComponent` : Add to your player controller class to call the `Update` function of the `SignificanceManager` if you don't want to (or cannot) do that from for example the client viewport class.
  - `USEGetSignificanceComponent` : Add to your actors that you want to register to the `SignificanceManager` and compute their significance.
  - `USEGetSignificanceInterface` : Add you your actors to allow them to override the functions of the component.

### Significance computation

1. If `bUseFixedSignificance` is set, then the significance is given by the property `FixedSignificance`
2. If the owner of the component implemements the interface `USEGetSignificanceInterface` then the significance is the result of the function `GetSignificance` calld on the actor
3. If the component is a blueprint and the function `K2_GetSignificance` has been implemented, then the significance is the result of this function
4. Else the significance is computed from the `SignificanceDistances` array 

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

1. Fork this repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Clone to your UE project's Plugins folder
4. Make your changes and test thoroughly
5. Submit a pull request

### Coding Standards
- Follow [Unreal Engine Coding Standards](https://docs.unrealengine.com/5.3/en-US/epic-cplusplus-coding-standards-for-unreal-engine/)
- Use clear, descriptive variable and function names
- Comment complex algorithms and public APIs

## Support

### Getting Help
- **Documentation**: [Full documentation](https://yourplugin-docs.com)

### Reporting Issues
Please report bugs and feature requests through [GitHub Issues](https://github.com/yourusername/yourplugin/issues).

**When reporting issues, please include:**
- Unreal Engine version
- Plugin version
- Platform (Windows/Mac/Linux)
- Steps to reproduce
- Expected vs actual behavior
- Relevant log files

### FAQ

**Q: Can I use this in commercial projects?**
A: Yes! See the [LICENSE](LICENSE) file for details.

**Q: How do I update the plugin?**
A: Download the new version and replace the plugin files, then recompile your project.

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for complete version history.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Made with ❤️ for the Unreal Engine community**

*If this plugin helped you, consider giving it a ⭐ on GitHub!*