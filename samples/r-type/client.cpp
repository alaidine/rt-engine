#include "RoarEngine.h"

Roar::Application *Roar::CreateApplication(int argc, char **argv) {
    Roar::ApplicationSpecification spec;
    spec.Title = "R-Type";
    spec.Title = "R-Type Server";
    
    Roar::Application rtype = new Roar::Application(spec);
    return rtype;
}
