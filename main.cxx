#include "QVTKApplication.h"

#include "GUI.h"

int main(int argc, char** argv)
{
  QVTKApplication app(argc, argv);
  //GUI widget;

  std::string vesselFile1 = argv[1];
  std::string tumorFile1 = argv[2];
  std::string vesselFile2 = argv[3];
  std::string tumorFile2 = argv[4];
  GUI widget(vesselFile1, tumorFile1, vesselFile2, tumorFile2);
  widget.show();


  return app.exec();
}
