#ifndef LAYOUTMANAGEDCOMPONENT_H_INCLUDED
#define LAYOUTMANAGEDCOMPONENT_H_INCLUDED

#include "LayoutManager.h"

class LayoutManagedComponent : public LayoutManager, public Component
{
public:
    LayoutManagedComponent(Orientation orientation);
    virtual void resized() override;
    virtual Constraints getSizeHint(Orientation orientation) const;
};

#endif
