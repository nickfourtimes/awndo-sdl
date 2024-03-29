# THE AMERICAN DREAM

## Are We Not Drawn Onward To New Era?

I made this for Kokoromi's [Gamma4](https://www.kickstarter.com/projects/1188957169/gamma4-kokoromi-brings-legendary-indie-game-showc) one-button showcase, which took place in San Francisco on 10 March 2010.

It wasn't selected, but it was later shown in Montréal, at Eastern Bloc's *[Housewarming Party](https://easternbloc.ca/en/housewarming-party)*, from 7 to 11 September 2011, and is also available on my [itch.io](https://nicknicknicknick.itch.io/the-american-dream-awndo) page.

An Android version is [also available](https://play.google.com/store/apps/details?id=org.newton64.TheAmericanDream), which had been ported to the [Orx engine](http://orx-project.org/) – though I've lost the code to both that one and a potential Unity port.

### A note on the code
The project makes use of [SDL 1.2](https://www.libsdl.org/download-1.2.php) specifically, along with the following related libraries:
* [SDL_image](https://www.libsdl.org/projects/SDL_image/release-1.2.html)
* [SDL_mixer](https://www.libsdl.org/projects/SDL_mixer/release-1.2.html)
* [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/release-1.2.html)

![image](assets/images/screenshots/title.png) ![image](assets/images/screenshots/work.png)

## Readme Archaeology

The following notes were written when the project was completed in 2010, and were mostly for my own benefit. I include them here in the interest of preservation.

### CONTROLS

This is a one-button game. The keys Z, X, or Space can be used as "the button," as well as the A button on an Xbox controller (or equivalent). Press the button to make a selection and advance dialogue. Press or hold the button in each of the minigames – tutorials are available for each.

### PROGRAMMING NOTES

* IMG_Load() should always be followed up by a SDL_DisplayFormat()!

### GAMEPLAY NOTES

* "DEAD" is inspired by Brian Eno's "Music for Airports" and a snowy morning.

### BIOPSY

* Some constraints good (all minigames within frame -> unified style!)
* Other constraints bad (minigames must be strictly diametric opposites -> limiting)

### COMPILATION NOTES

* WINDOWS
  * Compiled "Multithreaded DLL" (/MD)
  * [Obsolete] Linker ignores these libraries: libc.lib, libcmt.lib, libcd.lib, libcmtd.lib, msvcrtd.lib
  
## Links

* Android version on [Google Play](https://play.google.com/store/apps/details?id=org.newton64.TheAmericanDream).
* PC version on [itch.io](https://nicknicknicknick.itch.io/the-american-dream-awndo).
* The [TIGSource thread](https://forums.tigsource.com/index.php?topic=11158.0).
  
## History

* 10 March 2010
  * Initial release (GAMMA IV)
* 7 September 2011
  * Eastern Bloc Housewarming Party
* 29 November 2011
  * Android app release
* 12 June 2020
  * GitHub public release

---

Drink pairing: Cheap lager  
Music pairing: [_Ambient 1: Music for Airports_](http://en.wikipedia.org/wiki/Music_for_Airports), Brian Eno
