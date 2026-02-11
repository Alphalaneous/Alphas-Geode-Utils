# 1.2.0-beta.2
- Geode Alpha 2 support
- Performance Improvements
- Added $baseModify

# 1.2.0-beta.1
- Cleanup everything
- Added $classModify to use a base class instead of CCNode or CCObject
- `modify` method is not required anymore if you wish to merely use fields
- Removed some redundant utils that are now in Geode
- Added templated `getObjectName` method

# 1.1.8
- Quick fixes you'll never see as an end user
- Code cleanup

# 1.1.7
- Fix a Cocos2d-x 2.2.3 bug where for some godforsaken reason, autorelease can be called on a nullptr in CCDictionary::createWithContentsOfFile. Thank you cocos devs :)

# 1.1.6
- iOS rebuild
- New icon

# 1.1.5
- API fix

# 1.1.4
- Attempt to fix a crash

# 1.1.3
- Fix an oopsie

# 1.1.2
- Fix a leak and crash

# 1.1.1
- Separate Node and Object modify macros
    - $nodeModify now has a base class of CCNode
    - $objectModify has a base class of CCObject

# 1.1.0
- Fields and User Objects can now be used with CCObjects in the same way as CCNodes

# 1.0.5
- iOS support

# 1.0.4
- Add tags

# 1.0.3
- Add string variant for getLayer
- Fix a bug with getLayer

# 1.0.2
- Fix a typo

# 1.0.1
- Code cleanup

# 1.0.0
- Initial Release
