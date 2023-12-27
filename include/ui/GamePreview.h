//#ifndef GamePreview_h
//#define GamePreview_h
//
//#include <Arduino.h>
//#include "UIElement.h"
//#include "SimpleButton.h"
//
//class GamePreview : public SimpleButton {
//	public:
//		uint8_t gameId = _NO_GAME;
//		Callback<uint8_t> callback;
//
//		void handlePress() override {
//			depressed = !depressed;
//			render();
//			if (callback.function != nullptr) {
//				callback.function(gameId);
//			}
//		}
//
//		explicit GamePreview(uint8_t gameId) : gameId(gameId) {
//			width = 288;
//			height = 31;
//		}
//
//		void render() override {
//			DEBUG(F("render game preview for game id: ") << gameId);
//			Folder folder;
//			Title title;
//			Description description;
//
//			// try and draw a splash.bmp in place of the button (if exists)
//			Storage::begin();
//			const sd_err_code navCode = changeToGameDirectory(&folder, &title, &description);
//			Storage::end();
//			if (navCode != noSdError) {
//				ERROR(F("err can't navigate into potential game dir"));
//				return; // failure to navigate into the game folder
//			}
//
//			Storage::begin();
//			const sd_err_code imgCode = drawSplashImage(&folder);
//			Storage::end();
//			if (imgCode == noSdError) {
//				return; // if successful, exit render
//			}
//
//			// no splash.bmp file found, so draw the title in the button
//			DEBUG(F("simple button render"));
//			drawTitle(&title);
//		}
//
//		sd_err_code changeToGameDirectory(Folder* folder, Title* title, Description* desc) const {
//			const sd_err_code code = Storage::loadGameData(gameId, folder, title, desc);
//			if (code != noSdError) {
//				ERROR(F("failed to load game data"));
//				return code;
//			}
//			DEBUG("dir: " << folder->text << " -> " << title->text.text << " | " << desc->text);
//
//			Storage::setWorkingDirectoryToRoot();
//			Storage::setWorkingDirectory(folder->text);
//			return noSdError;
//		}
//
//		void drawTitle(Title* title) {
//			if (!title->isValid()) {
//				title->resetTitle();
//			}
//
//			text = new Text(*title);
//			SimpleButton::render();
//			delete text;
//		}
//
//		// If the user places a splash.bmp image in the root of the game dir, this will draw in the
//		// load games screen instead of the game title
//		sd_err_code drawSplashImage(Folder* folder) {
//			Storage::setWorkingDirectoryToRoot();
//			Storage::setWorkingDirectory(folder->text);
//
//			if (!Storage::fileExists("splash.bmp")) {
//				DEBUG(F("no splash image file"));
//				return errInvalidFilename;
//			}
//
//			// check the size of the bmp though
//			const sd_err_code bmpCode =
//					  bmpDraw("splash.bmp", this->x, this->y, this->width, this->height);
//			if (bmpCode != noSdError) {
//				return bmpCode;
//			}
//
//			return noSdError;
//		}
//};
//
//#endif