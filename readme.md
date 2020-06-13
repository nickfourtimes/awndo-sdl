# THE AMERICAN DREAM
## Are We Not Drawn Onward To New Era?

I made this for Kokoromi's [Gamma4](https://www.kickstarter.com/projects/1188957169/gamma4-kokoromi-brings-legendary-indie-game-showc) one-button showcase, showing in San Francisco in March 2010.

It wasn't selected, but it was later shown in Montréal, at Eastern Bloc's *[Housewarming Party](https://easternbloc.ca/en/housewarming-party)*, from 7 to 11 September 2011, and is also available as an executable on my [itch.io](https://nicknicknicknick.itch.io/the-american-dream-awndo) page.

The game requires the SDL, SDL_image, SDL_mixer, & SDL_ttf libraries to compile (I used SDL 2).

An Android version is [also available](https://play.google.com/store/apps/details?id=org.newton64.TheAmericanDream), having been ported to the [Orx engine](http://orx-project.org/). Both this version and a potentially-upcoming Unity version may one day join this project online.

## Readme Archaeology

The following notes were written when the project was completed in 2010, and were mostly for my own benefit. I include them here in the interest of preservation.

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
  