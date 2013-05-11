#include "GUI.h"

#include <QMenu>
#include <QDebug>

#include "QVTKInteractor.h"
#include "vtkActor.h"
#include "vtkColorTransferFunction.h"
#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkInteractorStyle.h"
#include "vtkParticleReader.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkTDxInteractorStyleCamera.h"
#include "vtkTDxInteractorStyleSettings.h"
#include "vtkTransform.h"



GUI::GUI(std::string v1, std::string t1, std::string v2, std::string t2)
{
  this->setupUi(this);

  // create a window to make it stereo capable and give it to QVTKWidget
  vtkRenderWindow* renwin = vtkRenderWindow::New();
  //renwin->StereoCapableWindowOn();

  // Activate 3DConnexion device only on the left render window.
  //qVTK1->SetUseTDx(true);

  qVTK1->SetRenderWindow(renwin);
  //renwin->Delete();

  const double angleSensitivity=0.02;
  const double translationSensitivity=0.001;

  QVTKInteractor *iren=qVTK1->GetInteractor();
  vtkInteractorStyle *s=
    static_cast<vtkInteractorStyle *>(iren->GetInteractorStyle());
  vtkTDxInteractorStyleCamera *t=
    static_cast<vtkTDxInteractorStyleCamera *>(s->GetTDxStyle());

  // add a renderer
  Ren1 = vtkRenderer::New();
  qVTK1->GetRenderWindow()->AddRenderer(Ren1);

  // add a popup menu for the window and connect it to our slot
  QMenu* popup1 = new QMenu(qVTK1);
  popup1->addAction("Background Grey");
  popup1->addAction("Background Black");
  popup1->addAction("Stereo Rendering");
  connect(popup1, SIGNAL(triggered(QAction*)), this, SLOT(color1(QAction*)));

  vtkSmartPointer<vtkParticleReader> leftVesselReader = vtkSmartPointer<vtkParticleReader>::New();
  leftVesselReader->SetDataByteOrderToBigEndian();
  leftVesselReader->Update();
  leftVesselReader->SetFileName(v1.c_str());
  vtkSmartPointer<vtkParticleReader> rightVesselReader = vtkSmartPointer<vtkParticleReader>::New();
  rightVesselReader->SetDataByteOrderToBigEndian();
  rightVesselReader->Update();
  rightVesselReader->SetFileName(v2.c_str());
  vtkSmartPointer<vtkPolyDataReader> leftLabelReader = vtkSmartPointer<vtkPolyDataReader>::New();
  leftLabelReader->SetFileName(t1.c_str());
  vtkSmartPointer<vtkPolyDataReader> rightLabelReader = vtkSmartPointer<vtkPolyDataReader>::New();
  rightLabelReader->SetFileName(t2.c_str());

  vtkSmartPointer<vtkPolyDataMapper> leftVesselMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  leftVesselMapper->SetInputConnection(leftVesselReader->GetOutputPort());
  leftVesselMapper->SetScalarRange(0,0.032786);
  vtkSmartPointer<vtkPolyDataMapper> rightVesselMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  rightVesselMapper->SetInputConnection(rightVesselReader->GetOutputPort());
  rightVesselMapper->SetScalarRange(0,0.032786);

  vtkSmartPointer<vtkPolyDataMapper> leftLabelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  leftLabelMapper->SetInputConnection(leftLabelReader->GetOutputPort());
  std::cout << "number of labels: " << leftLabelMapper->GetNumberOfPieces() << std::endl;
  leftLabelMapper->SetScalarRange(0, 10);
  vtkSmartPointer<vtkPolyDataMapper> rightLabelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  rightLabelMapper->SetInputConnection(rightLabelReader->GetOutputPort());
  std::cout << "number of labels: " << rightLabelMapper->GetNumberOfPieces() << std::endl;
  rightLabelMapper->SetScalarRange(0, 10);

  // Define some transforms
  vtkSmartPointer<vtkTransform> shrinkTransform = vtkSmartPointer<vtkTransform>::New();
  shrinkTransform->Scale(0.05, 0.05, 0.05);
  vtkSmartPointer<vtkTransform> rotateTransform = vtkSmartPointer<vtkTransform>::New();
  rotateTransform->RotateZ(180);

  double range[2];
  leftVesselMapper->GetScalarRange(range);

  vtkSmartPointer<vtkColorTransferFunction> vesselColor = vtkSmartPointer<vtkColorTransferFunction>::New();
  vesselColor->AddRGBPoint(0.0,0.0,0.0,1.0);
  vesselColor->AddRGBPoint(range[1] / 2, 1.0, 1.0,1.0);
  vesselColor->AddRGBPoint(0.032786,1.0,0.0,0.0);
  leftVesselMapper->SetLookupTable(vesselColor);
  rightVesselMapper->SetLookupTable(vesselColor);

  vtkSmartPointer<vtkActor> leftVesselActor = vtkSmartPointer<vtkActor>::New();
  leftVesselActor->SetMapper(leftVesselMapper);
  leftVesselActor->GetProperty()->SetPointSize(2);
  leftVesselActor->SetUserTransform(shrinkTransform);
  Ren1->AddActor(leftVesselActor);

  vtkSmartPointer<vtkActor> rightVesselActor = vtkSmartPointer<vtkActor>::New();
  rightVesselActor->SetMapper(rightVesselMapper);
  rightVesselActor->GetProperty()->SetPointSize(2);
  rightVesselActor->SetUserTransform(shrinkTransform);
  //Ren1->AddActor(leftVesselActor);

  vtkSmartPointer<vtkActor> leftLabelActor = vtkSmartPointer<vtkActor>::New();
  leftLabelActor->SetMapper(leftLabelMapper);
  leftLabelActor->SetUserTransform(rotateTransform);
  leftLabelActor->GetProperty()->SetOpacity(0.5);
  Ren1->AddActor(leftLabelActor);

  vtkSmartPointer<vtkActor> rightLabelActor = vtkSmartPointer<vtkActor>::New();
  rightLabelActor->SetMapper(rightLabelMapper);
  rightLabelActor->SetUserTransform(rotateTransform);
  rightLabelActor->GetProperty()->SetOpacity(0.5);
  //Ren1->AddActor(leftLabelActor);

  // create a window to make it stereo capable and give it to QVTKWidget
  renwin = vtkRenderWindow::New();
  renwin->StereoCapableWindowOn();

  qVTK2->SetUseTDx(true);
  qVTK2->SetRenderWindow(renwin);
  renwin->Delete();

  QVTKInteractor *iren2 = qVTK2->GetInteractor();
  vtkInteractorStyle *s2 = static_cast<vtkInteractorStyle *>(iren2->GetInteractorStyle());
  vtkTDxInteractorStyle *tt2 = s2->GetTDxStyle();
  tt2->SetSettings(t->GetSettings());

  // add a renderer
  Ren2 = vtkRenderer::New();
  qVTK2->GetRenderWindow()->AddRenderer(Ren2);

  Ren2->AddActor(rightLabelActor);
  Ren2->AddActor(rightVesselActor);


  // add a popup menu for the window and connect it to our slot
  QMenu* popup2 = new QMenu(qVTK2);
  popup2->addAction("Background Grey");
  popup2->addAction("Background Black");
  popup2->addAction("Stereo Rendering");
  connect(popup2, SIGNAL(triggered(QAction*)), this, SLOT(color2(QAction*)));

  Connections = vtkEventQtSlotConnect::New();

  // get right mouse pressed with high priority
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(),
                       vtkCommand::RightButtonPressEvent,
                       this,
                       SLOT(popup( vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                       popup1, 1.0);

  // get right mouse pressed with high priority
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(),
                       vtkCommand::RightButtonPressEvent,
                       this,
                       SLOT(popup( vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                       popup2, 1.0);

  // connect window enter event to radio button slot
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(),
                       vtkCommand::EnterEvent,
                       radio1,
                       SLOT(animateClick()));

  // connect window enter event to radio button slot
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(),
                       vtkCommand::EnterEvent,
                       radio2,
                       SLOT(animateClick()));

  // update coords as we move through the window
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(),
                       vtkCommand::MouseMoveEvent,
                       this,
                       SLOT(updateCoords(vtkObject*)));

  // update coords as we move through the window
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(),
                       vtkCommand::MouseMoveEvent,
                       this,
                       SLOT(updateCoords(vtkObject*)));

  Connections->PrintSelf(cout, vtkIndent());
}

GUI::~GUI()
{
  Ren1->Delete();
  Ren2->Delete();

  Connections->Delete();
}


void GUI::updateCoords(vtkObject* obj)
{
  // get interactor
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(obj);
  // get event position
  int event_pos[2];
  iren->GetEventPosition(event_pos);
  // update label
  QString str;
  str.sprintf("x=%d : y=%d", event_pos[0], event_pos[1]);
  coord->setText(str);
}

void GUI::popup(vtkObject * obj, unsigned long,
           void * client_data, void *,
           vtkCommand * command)
{
  // A note about context menus in Qt and the QVTKWidget
  // You may find it easy to just do context menus on right button up,
  // due to the event proxy mechanism in place.

  // That usually works, except in some cases.
  // One case is where you capture context menu events that
  // child windows don't process.  You could end up with a second
  // context menu after the first one.

  // See QVTKWidget::ContextMenuEvent enum which was added after the
  // writing of this example.

  // get interactor
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::SafeDownCast(obj);
  // consume event so the interactor style doesn't get it
  command->AbortFlagOn();
  // get popup menu
  QMenu* popupMenu = static_cast<QMenu*>(client_data);
  // get event location
  int* sz = iren->GetSize();
  int* position = iren->GetEventPosition();
  // remember to flip y
  QPoint pt = QPoint(position[0], sz[1]-position[1]);
  // map to global
  QPoint global_pt = popupMenu->parentWidget()->mapToGlobal(pt);
  // show popup menu at global point
  popupMenu->popup(global_pt);
}

void GUI::color1(QAction* color)
{
  if(color->text() == "Background Grey")
    Ren1->SetBackground(0.1,0.2,0.3);
  else if(color->text() == "Background Black")
    Ren1->SetBackground(0,0,0);
  else if(color->text() == "Stereo Rendering")
  {
    Ren1->GetRenderWindow()->SetStereoRender(!Ren1->GetRenderWindow()->GetStereoRender());
  }
  qVTK1->update();
}

void GUI::color2(QAction* color)
{
  if(color->text() == "Background Grey")
    this->Ren2->SetBackground(0.1,0.2,0.3);
  else if(color->text() == "Background Black")
    this->Ren2->SetBackground(0,0,0);
  else if(color->text() == "Stereo Rendering")
  {
    this->Ren2->GetRenderWindow()->SetStereoRender(!this->Ren2->GetRenderWindow()->GetStereoRender());
  }
  qVTK2->update();
}

// Stub methods for further robust UI development
void GUI::setVesselFile(std::string filename)
{
    vesselFile = filename;
}

void GUI::setTumorFile( std::string filename)
{
    tumorFile = filename;
}
