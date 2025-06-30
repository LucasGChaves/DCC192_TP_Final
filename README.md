# A RUFF QUEST

### 🎮 Description

**A Ruff Quest** is a top-down, 2D retro-style action RPG developed in **C++ using the SDL library**, targeting **PC platforms (Windows, Linux, and macOS)**. Inspired by SNES-era classics like The Legend of Zelda: A Link to the Past and Secret of Mana, the game features a charming pixel art aesthetic with 16x16 tiles and a fantasy medieval setting.
You play as an adventurer on a quest to rescue your loyal dog, who runs into a mysterious dungeon after hearing strange sounds coming from inside the cave.

The gameplay features a classic top-down view with 4-directional movement and real-time sword combat. The player has five lives and must defeat every enemy within the dungeon to win the game. A clean and informative HUD displays the remaining lives and score, which increases based on the number of defeated enemies.

Victory is achieved by clearing all enemies from the dungeon. The game ends in defeat if the player loses all lives.

### 💻 Running
- Insert `assets` folder into `Assets/Images/mapDrafts/`
- Insert `Level1.wav` and `MainMenu.wav` into `Assets/Sounds/`
  
### 🎮 Tutorial
- Start: Enter [Return]
- Movimentação em quatro direções: [← ↑ → ↓]
- Ataque: [barra de espaço]
- Pause: [ESC]

### ✅ Features to Test
During playtesting, please pay attention to the following features:
- 4-directional character movement and control responsiveness;
- Sword attack mechanics and enemy defeat logic;
- Enemy AI and movement behavior;
- Collision detection with walls and environment;
- Room/map transitions;
- HUD display (score counter and life tracker);
- Win condition: clearing all enemies;
- Game over condition: losing all five lives;
- Ambient audio and sound effects

🖼️ In-Game Screenshots:

[![Title](https://raw.githubusercontent.com/LucasGChaves/DCC192_TP_Final/dev/Assets/Demo/title.png "Title")](https://github.com/LucasGChaves/DCC192_TP_Final)
[![intro](https://raw.githubusercontent.com/LucasGChaves/DCC192_TP_Final/dev/Assets/Demo/intro.png "intro")](https://github.com/LucasGChaves/DCC192_TP_Final)
[![caveEntrance](https://raw.githubusercontent.com/LucasGChaves/DCC192_TP_Final/dev/Assets/Demo/caveEntrance.png "caveEntrance")](https://github.com/LucasGChaves/DCC192_TP_Final)
[![dungeonStage](https://raw.githubusercontent.com/LucasGChaves/DCC192_TP_Final/dev/Assets/Demo/dungeonStage.png "dungeonStage")](https://github.com/LucasGChaves/DCC192_TP_Final)

### 👥 Team Credits
- Adalberto Vieira – Base setup for project, Player actor, Skelleton actor, Dog actor, Enemy AI, HUD(kill counter), Main menu, Win and Lose menus, win and loss conditions and Testing and Debugging;
- Felipe Gonçalves – Player attack, Skelleton spawning, Win menu, Player-Skelleton collision, Player lifes, Player hit receiving and invencibility, Testing and Debbuging;
- Iago Rios – Level Design, HUD, Sound system, Spike gate and dog actors, Ending's Logic, Cinematics (Dog's automatic wandering on Scene 1) Testing and Debugging;
- Lucas Chaves – Base setup for project, Level Design, Level Building, Sound effects (Dog barking and Win Music), Collisions (Scenery, Player Bounding-Box), Camera, Scaling, Ending's Logic, Sprite Component adjustments, Cinematics (Automatic Leaving on Scene 1 and Automatic Entering on Scene 2), Testing and Debugging.

### 🛠️ Tools
- **Graphics**: [GIMP](https://www.gimp.org/), [Aseprite](https://www.aseprite.org/), [Tiled](https://www.mapeditor.org/), [Piskel](https://www.piskelapp.com/)  
- **Audio**: [FamiStudio](https://famistudio.org/)

### 🎨 Arts & Audio
**Assets sourced from:**
- [Itch.io](https://itch.io)
- [OpenGameArt](https://opengameart.org)

### 🎨 Visual Assets:
- [Kenmi – Cute Fantasy Pack](https://kenmi-art.itch.io/)
- [Admurin – Top Down Mobs Dog](https://admurin.itch.io/top-down-mobs-dog)
- [Pixelius Vita – Monster Fantasy Pack 01](https://pixelius-vita.itch.io/free-asset-pack-01)
- [Pixel Poem – Dungeon Tileset](https://pixel-poem.itch.io/dungeon-assetpuck)

### 🔊 Audio Assets:
- [Minifantasy – Dungeon Audio Pack](https://leohpaz.itch.io/minifantasy-dungeon-sfx-pack)
- [Yubatake – JRPG Collection](https://opengameart.org/content/jrpg-collection)
- [Canari Games - CanariPack 8BIT TopDown](https://canarigames.itch.io/canaripack-8bit-topdown)
- [OpenGameArt.org - User:remaxim - Win Music #2](https://opengameart.org/content/win-music-2)
  
### 🔤 Font:
- [Emhuo – Peaberry Font Family](https://emhuo.itch.io/peaberry-pixel-font)

All assets are either free or commercially usable, and properly credited according to their respective licenses. Only free license sprites have been added to the repository.
