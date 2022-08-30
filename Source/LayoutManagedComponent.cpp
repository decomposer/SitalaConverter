#include "LayoutManagedComponent.h"

LayoutManagedComponent::LayoutManagedComponent(Orientation orientation) :
    LayoutManager(this, orientation)
{

}

void LayoutManagedComponent::resized()
{
    Component::resized();
    LayoutManager::resized();
}

struct HintTypes
{
    bool absolute = true;
    bool percentage = true;
    bool none = true;
};

template <typename A, typename F> HintTypes hintTypes(
    LayoutManager::Orientation orientation, const A &children, float initial, F getValue)
{
    HintTypes types;

    for(auto child : children)
    {
        auto childHint = dynamic_cast<SizeHint *>(child);

        if(childHint)
        {
            auto hint = childHint->getSizeHint(orientation);
            auto value = getValue(hint);

            if(value != initial)
            {
                types.none = false;
            }

            if(value >= 0)
            {
                types.absolute &= true;
                types.percentage = false;
            }
            else
            {
                types.absolute = false;
                types.percentage &= true;
            }
        }
        else
        {
            types.percentage = false;
        }
    }

    return types;
}

LayoutManager::Constraints LayoutManagedComponent::getSizeHint(Orientation orientation) const
{
    auto children = Array<Component *>();

    for(auto i = 0; i < getNumChildComponents(); i++)
    {
        children.add(getChildComponent(i));
    }

    auto initial = Constraints();

    auto minimumHintTypes =
        hintTypes(orientation, children, initial.m_minimum,
                  [](const LayoutManager::Constraints &c) { return c.m_minimum; });

    auto preferredHintTypes =
        hintTypes(orientation, children, initial.m_preferred,
                  [](const LayoutManager::Constraints &c) { return c.m_preferred; });

    auto maximumHintTypes =
        hintTypes(orientation, children, initial.m_maximum,
                  [](const LayoutManager::Constraints &c) { return c.m_maximum; });

    auto toAbsolute = [this](float value) {
        return (value >= 0) ? value : (value * -1 * getWidth());
    };

    auto constraints = Constraints::fixed(0);

    for(auto child : children)
    {
        auto childHint = dynamic_cast<SizeHint *>(child);
        auto hint = childHint ? childHint->getSizeHint(orientation) : Constraints::fixed(0);

        if(!minimumHintTypes.percentage)
        {
            if(orientation == getOrientation())
            {
                constraints.m_minimum += toAbsolute(hint.m_minimum);
            }
            else
            {
                constraints.m_minimum = jmax(constraints.m_minimum, hint.m_minimum);
            }
        }

        if(!preferredHintTypes.percentage)
        {
            auto value = childHint ? toAbsolute(hint.m_preferred) :
                         ((orientation == Orientation::Horizontal) ?
                          child->getWidth() : child->getHeight());
            if(orientation == getOrientation())
            {
                constraints.m_preferred += value;
            }
            else
            {
                constraints.m_preferred = jmax(constraints.m_preferred, value);
            }
        }

        if(constraints.m_maximum >= 0)
        {
            if(hint.m_maximum >= 0)
            {
                if(orientation == getOrientation())
                {
                    constraints.m_maximum += hint.m_maximum;
                }
                else
                {
                    constraints.m_maximum = jmax(constraints.m_maximum, hint.m_maximum);
                }
            }
            else
            {
                constraints.m_maximum = 0;
            }
        }
    }

    if(minimumHintTypes.none)
    {
        constraints.m_minimum = initial.m_minimum;
    }
    if(preferredHintTypes.none)
    {
        constraints.m_preferred = initial.m_preferred;
    }
    if(maximumHintTypes.none)
    {
        constraints.m_maximum = initial.m_maximum;
    }

    return constraints;
}
