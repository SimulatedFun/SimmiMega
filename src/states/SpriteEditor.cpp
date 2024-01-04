#include "states/SpriteEditor.h"

/// This namespace handles the Sprite Editor state entirely. The progression is:
/// 1. Open the object selection tray and wait for user to select an object
/// 2. Load the sprite into the editor, where the user can draw/erase, copy, paste, invert, etc.
/// 3. Obj is saved on exit or on certain actions
namespace SpriteEditor {
	uint64_t clipboardStorage = 0;		// storage for the copy/paste functionality
	boolean selectedTool = pencilTool; // alternates b/w pencil and eraser

	GameObject* gameObject;	 // selected gameobject to edit the sprites from
	uint64_t* currentSprite; // pointer to the sprite/frame currently being edited

	// region UI Elements
	SpriteEditGrid* editArea;				  // the "drawing area" of the sprite editor
	SpriteFrameSelector *frame1, *frame2; // preview of frame1/frame2 displayed in both buttons
	RoundButton *exit, *pencil, *eraser, *copy, *paste, *invert, *clear, *depth; // regular buttons
	// endregion

	/// Resets variables, opens the choose object screen, and starts up the sprite editor
	void setup() {
        // pick a gameobject to edit the sprites for
        boolean cancelled = false;
        uint16_t gameObjectId = 0;
        ChooseObject::pick(false, &gameObjectId, &cancelled);
        if (cancelled) {
            state = MainMenuState;
            return;
        }

		gameObject = new GameObject(gameObjectId);
		gameObject->load(false);

		selectedTool = pencilTool;
		currentSprite = &gameObject->data.frame1;

		setupUI();
		draw();
		touch->clearQueue();
	}

	/// Allocates all the UI Elements for the sprite editor
	void setupUI() {
		INFO(F("setup UI"));
		UIHelper::clearActive();

		exit = new RoundButton(ExitButton);
		exit->setPosition(288, 0);
		exit->callback.bind(&callbackExit);
		UIHelper::registerUI(exit);

		editArea = new SpriteEditGrid();
		editArea->setPosition(4, 4);
		editArea->callback.bind(&callbackEditSprite);
		UIHelper::registerUI(editArea);

		frame1 = new SpriteFrameSelector();
		frame1->setPosition(247, 160);
		frame1->setData(gameObject, Frame1);
		frame1->callback.bind(&callbackChangeFrame);
		frame1->selected = true;
		UIHelper::registerUI(frame1);

		frame2 = new SpriteFrameSelector();
		frame2->setPosition(247, 204);
		frame2->setData(gameObject, Frame2);
		frame2->callback.bind(&callbackChangeFrame);
		frame2->selected = false;
		UIHelper::registerUI(frame2);

		pencil = new RoundButton(PencilButton);
		pencil->depressed = true;
		pencil->setPosition(288, 32);
		pencil->callback.bind(&callbackPencil);
		UIHelper::registerUI(pencil);

		eraser = new RoundButton(EraserButton);
		eraser->setPosition(288, 56);
		eraser->callback.bind(&callbackEraser);
		UIHelper::registerUI(eraser);

		clear = new RoundButton(ClearButton);
		clear->setPosition(288, 80);
		clear->callback.bind(&callbackClear);
		UIHelper::registerUI(clear);

		copy = new RoundButton(CopyButton);
		copy->setPosition(288, 112);
		copy->callback.bind(&callbackCopy);
		UIHelper::registerUI(copy);

		paste = new RoundButton(PasteButton);
		paste->setPosition(288, 136);
		paste->callback.bind(&callbackPaste);
		UIHelper::registerUI(paste);

		invert = new RoundButton(InvertButton);
		invert->setPosition(288, 168);
		invert->callback.bind(&callbackInvert);
		UIHelper::registerUI(invert);

		depth = new RoundButton(DepthButton);
		depth->setPosition(252, 32);
		if (gameObject->isHighlighted()) {
			depth->depressed = false;
		} else {
			depth->depressed = true;
		}
		depth->callback.bind(&SpriteEditor::callbackChangePalette);
		UIHelper::registerUI(depth);
	}

	/// Binds to the main sprite editing area to handle the user drawing or erasing
	/// Updates the sprite in the main edit area and in either of the frame preview areas
	void callbackEditSprite(uint8_t index) {
		if (testBit(*currentSprite, index) == selectedTool) {
			// if pixel is already the value to which we're trying to set it to, do nothing
			return;
		}
		setBit(currentSprite, index, selectedTool); // changes the bit value in the sprite
		drawSpritePixel(index);							  // draw the pixel change in the main edit area
		if (frame1->selected) { // draw the pixel change also in the frame preview UI area
			frame1->drawPreviewPixel(index);
		} else {
			frame2->drawPreviewPixel(index);
		}
	}

	/// Handles the user switching frames to edit the other frame instead
	void callbackChangeFrame(Frame inFrame) {
		INFO(F("callbackChangeFrame to: ") << inFrame);

		// if we're trying to switch a frame that is already selected, do nothing
		if (inFrame == Frame1 and currentSprite == &gameObject->data.frame1) {
			return;
		}
		if (inFrame == Frame2 and currentSprite == &gameObject->data.frame2) {
			return;
		}

		if (inFrame == Frame1) {
			currentSprite = &gameObject->data.frame1;
			frame1->selected = true;
			frame2->selected = false;
			frame1->render();
			frame2->render();
			drawSpriteEditArea();
			return;
		}

		currentSprite = &gameObject->data.frame2;
		frame1->selected = false;
		frame2->selected = true;
		frame1->render();
		frame2->render();
		drawSpriteEditArea();
	}

	/// Binds as the callback from the exit button
	void callbackExit(RoundButton&) {
		exit->depressed = true;
		exit->render();
		gameObject->save();
		deallocate();
		oldState = MainMenuState;
        state = SpriteEditorState;
		INFO(F("exiting the sprite editor state"));
	}

	/// Handles the user selecting the pencil tool
	void callbackPencil(RoundButton&) {
		if (selectedTool == pencilTool) {
			return;
		}
		INFO(F("switch to pencil"));
		selectedTool = pencilTool;
		pencil->depressed = true;
		eraser->depressed = false;
		pencil->render();
		eraser->render();
	}

	/// Handles the user selecting the eraser tool
	void callbackEraser(RoundButton&) {
		if (selectedTool == eraserTool) {
			return;
		}
		INFO(F("switch to eraser"));
		selectedTool = eraserTool;
		eraser->depressed = true;
		eraser->render();
		pencil->depressed = false;
		pencil->render();
	}

	/// Draws one of the sprite edit area "pixels" based on the index of the pixel
	/// Call this for every pixel in the sprite and it will draw the full sprite
	/// Re-used by the tray element to only draw edited "pixels" and also to do the
	/// initial first draw
	constexpr uint8_t sizeOfPixel = 29;			// 29px is the width of each sprite edit area "pixel"
	constexpr uint8_t drawingAreaXOffset = 4; // 4px is the offset of the sprite edit area
	void drawSpritePixel(uint16_t index) {
		const uint8_t col = index % 8;
		const uint8_t row = index / 8;
		const uint16_t xPos = drawingAreaXOffset + col * sizeOfPixel;
		const uint16_t yPos = drawingAreaXOffset + row * sizeOfPixel;
		const boolean filledPixel = testBit(*currentSprite, index);
		if (filledPixel) {
			display->fillRectangle(xPos, yPos, sizeOfPixel, sizeOfPixel, foregroundColor(gameObject));
		} else {
			display->fillRectangle(xPos, yPos, sizeOfPixel, sizeOfPixel, BLACK);
		}
	}

	/// Draws the sprite edit area as well as the two frame previews
	void drawSpriteEditArea() {
		INFO(F("drawSpriteEditArea"));
		for (uint8_t i = 0; i < 64; i++) {
			drawSpritePixel(i);
		}
	}

	uint16_t foregroundColor(GameObject* obj) {
		if (obj->isHighlighted()) {
			return WHITE;
		}
		return DARK_GREY;
	}

	void deallocate() {
		UIHelper::clearActive();
		delete editArea;
		delete frame1;
		delete frame2;
		delete exit;
		delete pencil;
		delete eraser;
		delete copy;
		delete paste;
		delete invert;
		delete clear;
		delete depth;

		delete gameObject;
	}

	void loop() {
		UIHelper::loop();
	}

	void draw() {
		DrawingUtils::fill(RGB565(0x574b67));
		// TODO: preview of animated sprite at real size / animated?
		UIHelper::render();
		drawSpriteEditArea();
	}

	void callbackChangePalette(RoundButton&) {
		if (depth->depressed) {
			depth->depressed = false;
			depth->render();
			gameObject->set_IsHighlighted(true);

		} else {
			depth->depressed = true;
			depth->render();
			gameObject->set_IsHighlighted(false);
		}
		drawSpriteEditArea();
		frame1->render();
		frame2->render();
	}

	void callbackInvert(RoundButton&) {
		*currentSprite = ~*currentSprite;
		invert->depressed = true;
		invert->render();

		if (frame1->selected) {
			frame1->render();
		} else {
			frame2->render();
		}
		drawSpriteEditArea();

		invert->depressed = false;
		invert->render();
	}

	void callbackCopy(RoundButton&) {
		copy->depressed = true;
		copy->render();

		INFO(F("copied the sprite to clipboard"));
		clipboardStorage = *currentSprite;

		delay(200);
		copy->depressed = false;
		copy->render();
	}

	void callbackPaste(RoundButton&) {
		paste->depressed = true;
		paste->render();

		INFO(F("pasted from clipboard"));
		*currentSprite = clipboardStorage;
		if (frame1->selected) {
			frame1->render();
		} else {
			frame2->render();
		}
		drawSpriteEditArea();

		delay(200);
		paste->depressed = false;
		paste->render();
	}

	/// Clears the frame being edited
	void callbackClear(RoundButton&) {
		clear->depressed = true;
		clear->render();

		*currentSprite = 0;
		frame1->render();
		frame2->render();

		delay(200);
		clear->depressed = false;
		clear->render();
		drawSpriteEditArea();
	}

} // namespace SpriteEditor
