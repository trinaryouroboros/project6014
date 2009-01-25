// generated by Fast Light User Interface Designer (fluid) version 1.0102

#ifndef synchronizer_h
#define synchronizer_h
#include <FL/Fl.H>
extern void NextFrame ();
extern void PauseFrame ();
extern void GetTracks(int value);
extern void StartPlayback();
extern void StopPlayback();
extern void RestartFrame();
extern void DoExit();
extern void UpdatePlayTime(int);
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

class Synchronizer {
public:
  Synchronizer();
  Fl_Window *MainWindow;
  Fl_Menu_Bar *MainMenu;
  static Fl_Menu_Item menu_MainMenu[];
  static Fl_Menu_Item *File;
  static Fl_Menu_Item *Exit;
private:
  inline void cb_Exit_i(Fl_Menu_*, void*);
  static void cb_Exit(Fl_Menu_*, void*);
public:
  Fl_Output *CurrentTrack;
  Fl_Output *CurrentFrame;
  Fl_Output *PlayTime;
  Fl_Output *TextScreen;
  Fl_Slider *Slider;
  Fl_Browser *RaceChooser;
private:
  inline void cb_RaceChooser_i(Fl_Browser*, void*);
  static void cb_RaceChooser(Fl_Browser*, void*);
public:
  Fl_Check_Browser *TrackSelector;
private:
  inline void cb_TrackSelector_i(Fl_Check_Browser*, void*);
  static void cb_TrackSelector(Fl_Check_Browser*, void*);
public:
  Fl_Button *ChooseAllButton;
private:
  inline void cb_ChooseAllButton_i(Fl_Button*, void*);
  static void cb_ChooseAllButton(Fl_Button*, void*);
public:
  Fl_Button *ClearAllButton;
private:
  inline void cb_ClearAllButton_i(Fl_Button*, void*);
  static void cb_ClearAllButton(Fl_Button*, void*);
public:
  Fl_Button *PauseButton;
private:
  inline void cb_PauseButton_i(Fl_Button*, void*);
  static void cb_PauseButton(Fl_Button*, void*);
public:
  Fl_Button *NextButton;
private:
  inline void cb_NextButton_i(Fl_Button*, void*);
  static void cb_NextButton(Fl_Button*, void*);
public:
  Fl_Button *StartButton;
private:
  inline void cb_StartButton_i(Fl_Button*, void*);
  static void cb_StartButton(Fl_Button*, void*);
public:
  Fl_Button *RestartButton;
private:
  inline void cb_RestartButton_i(Fl_Button*, void*);
  static void cb_RestartButton(Fl_Button*, void*);
public:
  Fl_Button *StopButton;
private:
  inline void cb_StopButton_i(Fl_Button*, void*);
  static void cb_StopButton(Fl_Button*, void*);
public:
  void show(int argc, char **argv);
};
#endif
