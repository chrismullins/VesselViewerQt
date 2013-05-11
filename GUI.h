#ifndef _GUI_h
#define _GUI_h

#include <QMainWindow>
#include "ui_GUI.h"

class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;

class GUI : public QMainWindow, public Ui::GUI
{
  Q_OBJECT
public:
  GUI(std::string v1, std::string t1, std::string v2, std::string t2);
  ~GUI();

  void setVesselFile( std::string filename);
  void setTumorFile( std::string filename);
  std::string getTumorFile(void);
  std::string getVesselFile(void);

  std::string vesselFile;
  std::string tumorFile;

public slots:
  void updateCoords(vtkObject*);
  void popup(vtkObject * obj, unsigned long,
             void * client_data, void *,
             vtkCommand * command);
  void color1(QAction*);
  void color2(QAction*);


protected:
  vtkRenderer* Ren1;
  vtkRenderer* Ren2;
  vtkEventQtSlotConnect* Connections;
};



#endif // _GUI_h

