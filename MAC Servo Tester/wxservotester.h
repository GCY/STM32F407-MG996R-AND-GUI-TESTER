#ifndef __WX_ARDUINO__
#define __WX_ARDUINO__

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "serialport.h"
#include "connectargsdlg.h"

class App:public wxApp
{
   public:
      bool OnInit();
};

class Frame:public wxFrame
{
   public:
      Frame(const wxString&);

      void OnSendData(wxTimerEvent&);
      void OnConnectDevice(wxCommandEvent&);
      void OnReleaseDevice(wxCommandEvent&);
      void OnCenter(wxCommandEvent&);
      void OnExit(wxCommandEvent&);

   private:

      enum{
	 ID_SEND_DATA = 100,
	 ID_CONNECT_DEVICE,
	 ID_RELEASE_DEVICE,
	 ID_EXIT,
	 ID_CENTER
      };

      static const unsigned int MAX = 2;
      static const unsigned int PULSE_MAX = 2500; //us
      static const unsigned int PULSE_MIN = 500;  //us

      wxMenu *device_path;

      wxTimer timer;

      SerialPort serial;

      wxSlider *servos[MAX];
      wxBoxSizer *box[MAX];
      wxStaticText *servos_text[MAX];
      //unsigned char data[MAX];

      DECLARE_EVENT_TABLE()
};

#endif
