#include "Layer.h"

#include "Application.h"

namespace Roar {

void Layer::QueueTransition(std::unique_ptr<Layer> toLayer) {
    /*

    // TODO: don't do this
    auto &layerStack = Application::Get().m_LayerStack;
    for (auto &layer : layerStack) {
        if (layer.get() == this) {
            layer = std::move(toLayer);
            return;
        }
    }

    */

    // Defer the actual replacement to the Application so the current layer
    // instance is not destroyed while one of its member functions (e.g.
    // OnRender) is still executing.
    Application::Get().QueueTransition(std::move(toLayer), this);
}

} // namespace Roar
