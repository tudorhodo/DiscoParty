Set Sound = CreateObject("WMPlayer.OCX.7")
strSong = Wscript.Arguments.Item(0)
Sound.URL = strSong
Sound.settings.volume = 30
Sound.settings.setMode "loop", True
Sound.Controls.play
do while Sound.currentmedia.duration = 0
wscript.sleep 100
loop
wscript.sleep (int(Sound.currentmedia.duration)+1)*1000