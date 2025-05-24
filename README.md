# Alpha's Geode Utils

Miscellaneous Geode utils to make development easier:

## Modifying any CCObject

Familiar modify syntax as Geode, supporting fields. `class $objectModify(SomeObject)` or `class $nodeModify(SomeNode)`
You can name the modified node/object the same way as well `class $objectModify(MySomeObject, SomeObject)` or `class $nodeModify(MySomeNode, SomeNode)`

$nodeModify and $objectModify do the same thing, except $nodeModify is a base of CCNode while $objectModify is a base of CCObject

To use this, you will create a `void modify()` method within that class and inside of there you can change the node to your hearts content. You can use the fields struct just like in Geode to add fields if needed. 

To edit the priority (lets say another mod modifies the same node using this) you can add `static int modifyPrio()` to the class which should return an integer value that is the priority you wish to set. 

**Full Example modifying BetterInfo's CustomCreatorLayer:**

```c++
class $nodeModify(MyCustomCreatorLayer, CustomCreatorLayer) {

	static int modifyPrio() {
		return 10;
	}

	struct Fields {
		int m_number = 0;
	};

	void modify() {
		
		CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_playBtn_001.png");
		CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyCustomCreatorLayer::onEpicButton));
		btn->setID("epic-button"_spr);

		if (CCMenu* creatorButtonsMenu = typeinfo_cast<CCMenu*>(getChildByID("cvolton.betterinfo/creator-buttons-menu"))) {
			creatorButtonsMenu->addChild(btn);
			creatorButtonsMenu->updateLayout();
		}
	}

	void onEpicButton(CCObject* obj) {
		log::info("m_number {}", m_fields->m_number);
		m_fields->m_number++;
	}
};
```

## Modifying Base Classes of CCNodes

Using $baseModify, you can modify all nodes that inheret some class. In the following example, we modify all FLAlertLayers, not just FLAlertLayer itself. These are different than the above $nodeModify and $cobjectModify, as it requires a valid type to be passed in, thus it does not natively support modifying other mod's nodes without declaring the class beforehand.

```c++
class $baseModify(MyFLAlertLayer, FLAlertLayer) {

	void modify() {
		setScale(2.f);
	}

}
```

If you still wish to modify another mod's nodes by their base class, you can do the following.
Let's say we had a node called AlphasEpicNode, which in itself inherets CCNode, that a lot of nodes from AlphasEpicMod inherit. Because we are defining it beforehand (make sure the definition matches that of how it is in said mod), the mod can deduce it and it's own base class names and it will work as expected.

```c++
class AlphasEpicNode : public CCNode {};
class $baseModify(MyAlphasEpicNode, AlphasEpicNode) {

	void modify() {
		setScale(2.f);
	}

}
```

## General Utils

### AlphaUtils::Cocos namespace:

Getting sprites while ignoring Texture Loader fallback:
`std::optional<cocos2d::CCSprite*> getSprite(const char* sprName)`
`std::optional<cocos2d::CCSprite*> getSpriteByFrameName(const char* sprFrameName)`

Getting a layer from the scene even during transition:
`std::optional<cocos2d::CCNode*> getLayer()`
`std::optional<cocos2d::CCNode*> getLayerByClassName(std::string className)`

Setting a node color by hex code:
`bool setColorByHex(cocos2d::CCRGBAProtocol* node, std::string colorHex)`

Checking if a parent node contains a node anywhere in a tree:
`bool hasNode(cocos2d::CCNode* child, cocos2d::CCNode* parent)`

Getting a child by class name dynamically:
`std::optional<cocos2d::CCNode*> getChildByClassName(cocos2d::CCNode* node, std::string name, int index = 0)`

Getting a node's class name:
`std::string getClassName(cocos2d::CCObject* obj, bool removeNamespace = false)`

### AlphaUtils::Misc namespace:

Getting a random number:
`int getRandomNumber(int lower, int upper)`
