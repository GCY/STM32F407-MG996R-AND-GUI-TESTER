#include "wxservotester.h"

IMPLEMENT_APP(App)
DECLARE_APP(App)

bool App::OnInit()
{
   wxFrame *frame = new Frame(wxT("wxServoTester"));

   frame->Show(true);

   return true;
}

BEGIN_EVENT_TABLE(Frame,wxFrame)
   EVT_MENU(ID_EXIT,Frame::OnExit)
   EVT_MENU(ID_CENTER,Frame::OnCenter)
   EVT_MENU(ID_CONNECT_DEVICE,Frame::OnConnectDevice)
   EVT_MENU(ID_RELEASE_DEVICE,Frame::OnReleaseDevice)
   EVT_TIMER(ID_SEND_DATA,Frame::OnSendData)
END_EVENT_TABLE()

Frame::Frame(const wxString &title):wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU),timer(this,ID_SEND_DATA)
{
   timer.Stop();

   wxMenu *file = new wxMenu;
   file->Append(ID_EXIT,wxT("E&xit\tAlt-e"),wxT("Exit"));
   wxMenu *tools = new wxMenu;
   tools->Append(ID_CONNECT_DEVICE,wxT("C&onnect Device\tAlt-c"),wxT("Connect Device"));
   tools->Append(ID_RELEASE_DEVICE,wxT("R&elease Device\tAlt-r"),wxT("Release Device"));
   wxMenu *setting = new wxMenu;
   setting->Append(ID_CENTER,wxT("C&enter\tAlt-m"),wxT("Center"));

   wxMenuBar *bar = new wxMenuBar;
   bar->Append(file,wxT("File"));
   bar->Append(tools,wxT("Tools"));
   bar->Append(setting,wxT("Setting"));
   SetMenuBar(bar);

   wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(top);

   for(int i = 0;i < MAX;++i){
      box[i] = new wxBoxSizer(wxHORIZONTAL);
      top->Add(box[i],0,wxALIGN_CENTER_HORIZONTAL | wxALL,5);

      wxString text;
      text.Printf("Servo %d：",i + 1);
      servos_text[i] = new wxStaticText(this,wxID_STATIC,text,wxDefaultPosition,wxDefaultSize,0);
      box[i]->Add(servos_text[i],0,wxALIGN_CENTER_HORIZONTAL |  wxALIGN_CENTER_VERTICAL,5);

      servos[i] = new wxSlider(this,wxID_ANY,(PULSE_MAX+PULSE_MIN)/2,PULSE_MIN,PULSE_MAX,wxDefaultPosition,wxSize(200,-1),wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
      box[i]->Add(servos[i],0,wxALIGN_CENTER_HORIZONTAL | wxALL,5);
   }

   CreateStatusBar(1);
   SetStatusText(wxT("wxServoTester"));

   top->Fit(this);
   top->SetSizeHints(this);
}

void Frame::OnSendData(wxTimerEvent &event)
{
   unsigned char data[255] = {""};
   std::string pwm;

   pwm.append("[");
   for(int i = 0;i < MAX;++i){
      std::string s = std::to_string(servos[i]->GetValue());
      pwm.append(s);
      if(i+1 != MAX){
	 pwm.append(",");
      }
      else{
	 pwm.append("]");
      }
   }

   int size = pwm.length();

   //wxLogDebug(wxT("%s"),pwm);
   strcpy((char*)data,pwm.c_str());
   //wxLogDebug(wxT("%s"),(char*)data);
   serial.Write(data,size+1);
}

void Frame::OnConnectDevice(wxCommandEvent &event)
{
    ConnectArgsDialog dlg(this,wxID_ANY,wxT("Connect"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);

    if(dlg.ShowModal() == wxID_OK){
       serial.Open(dlg.GetDevicePath().mb_str());
       serial.SetBaudRate(wxAtoi(dlg.GetBaudRate()));
       serial.SetParity(8,1,'N');

       timer.Start(30);
    }
}

void Frame::OnReleaseDevice(wxCommandEvent &event)
{
   serial.Close();
   timer.Stop();
}

void Frame::OnCenter(wxCommandEvent &event)
{
   for(int i = 0;i < MAX;++i){
      servos[i]->SetValue((PULSE_MAX+PULSE_MIN)/2);
   }
}

void Frame::OnExit(wxCommandEvent &event)
{
   timer.Stop();
   Close();
}
