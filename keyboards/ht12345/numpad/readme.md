# numpad

18 key numpad with 2 rotary encoders and OLED display, designed around the Pro Micro. 

For more information regarding the keyboard, please visit the [website](https://kb.ht12345.xyz/np0) or [GitHub Repo](https://github.com/howardt12345/numpad).

* Keyboard Maintainer: [ht12345](https://github.com/howardt12345)
* Hardware Supported: numpad0 PCB (promicro, rp2040)
* Hardware Availability: limited prototype quantities

Make example for this keyboard (after setting up your build environment):

    make ht12345/numpad/promicro:vial
    
    make ht12345/numpad/rp2040:vial:uf2 CTPIM=yes

Flashing example for this keyboard:

    make ht12345/numpad/promicro:vial:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
