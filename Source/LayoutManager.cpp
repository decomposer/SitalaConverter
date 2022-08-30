#include "LayoutManager.h"

// #define LAYOUTMANAGER_DEBUG_SPACERS

const LayoutManager::Constraints Drawing::StandardSpacerConstraints =
    LayoutManager::Constraints::fixed(Drawing::DefaultSpace);

const LayoutManager::Constraints Drawing::ControlHeightConstraints =
    LayoutManager::Constraints().setPreferred(LayoutManager::Unit::Absolute,
                                              Drawing::ControlHeight);

class Spacer : public Component
{
public:
    Spacer(int no) : Component(String("Spacer ") + String(no))
    {
        setInterceptsMouseClicks(false, false);
    }
#ifdef LAYOUTMANAGER_DEBUG_SPACERS
    virtual void paint(Graphics &g) override
    {
        g.setColour(Colours::red);
        g.drawRect(1, 1, getWidth() - 2, getHeight() - 2);
    }
#endif
};

LayoutManager::Constraints::Constraints() :
    m_minimum(0),
    m_preferred(-1.0),
    m_maximum(-1.0)
{

}

LayoutManager::Constraints &LayoutManager::Constraints::setMinimum(Unit unit, float value)
{
    m_minimum = (unit == Unit::Percentage ? -1.0f : 1.0f) * value;
    return *this;
}

LayoutManager::Constraints &LayoutManager::Constraints::setPreferred(Unit unit, float value)
{
    m_preferred = (unit == Unit::Percentage ? -1.0f : 1.0f) * value;
    return *this;
}

LayoutManager::Constraints &LayoutManager::Constraints::setMaximum(Unit unit, float value)
{
    m_maximum = (unit == Unit::Percentage ? -1.0f : 1.0f) * value;
    return *this;
}

float LayoutManager::Constraints::getMinimum(Unit *unit) const
{
    if(unit)
    {
        *unit = m_minimum < 0.0f ? Unit::Percentage : Unit::Absolute;
    }

    return m_minimum < 0.0f ? m_minimum * -1.0f : m_minimum;
}

float LayoutManager::Constraints::getPreferred(Unit *unit) const
{
    if(unit)
    {
        *unit = m_preferred < 0.0f ? Unit::Percentage : Unit::Absolute;
    }

    return m_preferred < 0.0f ? m_preferred * -1.0f : m_preferred;
}

float LayoutManager::Constraints::getMaximum(Unit *unit) const
{
    if(unit)
    {
        *unit = m_maximum < 0.0f ? Unit::Percentage : Unit::Absolute;
    }

    return m_maximum < 0.0f ? m_maximum * -1.0f : m_maximum;
}

LayoutManager::Constraints &LayoutManager::Constraints::set(Type type, Unit unit, float value)
{
    switch(type)
    {
    case Type::Minimum:
        return setMinimum(unit, value);
    case Type::Preferred:
        return setPreferred(unit, value);
    case Type::Maximum:
        return setMaximum(unit, value);
    };

    return *this;
}

LayoutManager::Constraints LayoutManager::Constraints::fixed(float value)
{
    return Constraints()
        .setMinimum(Unit::Absolute, value)
        .setPreferred(Unit::Absolute, value)
        .setMaximum(Unit::Absolute, value);
}

LayoutManager::Constraints LayoutManager::Constraints::percentage(float value)
{
    return Constraints()
        .setMinimum(Unit::Percentage, value)
        .setPreferred(Unit::Percentage, value)
        .setMaximum(Unit::Percentage, value);
}

bool LayoutManager::Constraints::operator==(const Constraints &other) const
{
    return (m_minimum == other.m_minimum &&
            m_preferred == other.m_preferred &&
            m_maximum == other.m_maximum);
}

LayoutManager::LayoutManager(Component *managed, Orientation orientation) :
    m_managed(managed),
    m_orientation(orientation)
{

}

LayoutManager::~LayoutManager()
{
    for(auto item : m_items)
    {
        item.component->removeComponentListener(this);
    }
}

void LayoutManager::resized()
{
    std::vector<Component *> components(static_cast<size_t>(m_items.size()));

    for(auto i = 0; i < m_items.size(); i++)
    {
        components[(size_t)i] = m_items[i].component;
    }

    m_layout.layOutComponents(
        components.data(),
        m_items.size(),
        m_borders.getLeft(),
        m_borders.getTop(),
        m_managed->getWidth() - m_borders.getLeft() - m_borders.getRight(),
        m_managed->getHeight() - m_borders.getTop() - m_borders.getBottom(),
        m_orientation == Orientation::Vertical,
        true);
}

void LayoutManager::setBorderSizes(const BorderSize<int> &borders)
{
    m_borders = borders;
}

BorderSize<int> LayoutManager::getBorderSizes() const
{
    return m_borders;
}

void LayoutManager::prependComponent(Component *component, const Constraints &constraints,
                                     WithSpacer withSpacer)
{
    if(withSpacer == WithSpacer::Standard)
    {
        insertItemWithSpacer(Item(Item::Type::Component, component), constraints, 0);
    }
    else
    {
        insertItem(Item(Item::Type::Component, component), constraints, 0);
    }
}

void LayoutManager::prependComponent(Component *component, WithSpacer withSpacer)
{
    prependComponent(component, Constraints(), withSpacer);
}

void LayoutManager::appendComponent(Component *component, const Constraints &constraints,
                                    WithSpacer withSpacer)
{
    if(withSpacer == WithSpacer::Standard)
    {
        insertItemWithSpacer(Item(Item::Type::Component, component), constraints);
    }
    else
    {
        insertItem(Item(Item::Type::Component, component), constraints);
    }
}

void LayoutManager::appendComponent(Component *component, WithSpacer withSpacer)
{
    appendComponent(component, Constraints(), withSpacer);
}

void LayoutManager::insertComponentBefore(Component *before, Component *component,
                                          const Constraints &constraints, WithSpacer withSpacer)
{
    jassert(m_items.contains(Item(Item::Type::Component, before)) || !before);

    auto index = before ? m_items.indexOf(Item(Item::Type::Component, before)) : -1;

    if(withSpacer == WithSpacer::Standard)
    {
        insertItemWithSpacer(Item(Item::Type::Component, component), constraints, index);
    }
    else
    {
        insertItem(Item(Item::Type::Component, component), constraints, index);
    }
}

void LayoutManager::insertComponentBefore(Component *before, Component *component,
                                         WithSpacer withSpacer)
{
    insertComponentBefore(before, component, Constraints(), withSpacer);
}

void LayoutManager::insertComponentAfter(Component *after, Component *component,
                                         const Constraints &constraints, WithSpacer withSpacer)
{
    jassert(m_items.contains(Item(Item::Type::Component, after)) || !after);

    auto index = after ? m_items.indexOf(Item(Item::Type::Component, after)) + 1 : 0;

    if(withSpacer == WithSpacer::Standard)
    {
        insertItemWithSpacer(Item(Item::Type::Component, component), constraints, index);
    }
    else
    {
        insertItem(Item(Item::Type::Component, component), constraints, index);
    }
}

void LayoutManager::insertComponentAfter(Component *after, Component *component,
                                         WithSpacer withSpacer)
{
    insertComponentAfter(after, component, Constraints(), withSpacer);
}

Component *LayoutManager::removeComponent(Component *component)
{
    auto index = m_items.indexOf(Item(Item::Type::Component, component));

    if(index < 0)
    {
        return nullptr;
    }

    component->removeComponentListener(this);

    auto spacerIndex = (index == 0 && m_spacers.size() > 0) ? 1 : index - 1;

    if(spacerIndex >= 0 && m_items[spacerIndex].type == Item::Type::StandardSpacer)
    {
        removeItem(spacerIndex);
    }

    removeItem(m_items.indexOf(Item(Item::Type::Component, component)));

    resized();

    return component;
}

void LayoutManager::clear()
{
    m_layout.clearAllItems();

    for(auto item : m_items)
    {
        item.component->removeComponentListener(this);
    }

    m_items.clear();

    m_spacers.clear();
    m_resizeBars.clear();
}

void LayoutManager::setConstraints(Component *component, const Constraints &constraints)
{
    auto index = m_items.indexOf(Item(Item::Type::Component, component));

    if(index != -1)
    {
        m_layout.setItemLayout(index,
                               constraints.m_minimum,
                               constraints.m_preferred,
                               constraints.m_maximum);
    }
}

void LayoutManager::addSpacer(const Constraints &constraints)
{
    insertItem(Item(Item::Type::UserSpacer, new Spacer(m_spacers.size())), constraints);
}

void LayoutManager::addResizeBar()
{
    auto bar = new StretchableLayoutResizerBar(
        &m_layout, m_items.size(), m_orientation == LayoutManager::Orientation::Horizontal);
    insertItem(Item(Item::Type::ResizeBar, bar), Drawing::StandardSpacerConstraints);
}

void LayoutManager::insertItem(const Item &item, Constraints constraints, int index)
{
    if(constraints == Constraints())
    {
        auto sizeHint = dynamic_cast<SizeHint *>(item.component);

        if(sizeHint)
        {
            constraints = sizeHint->getSizeHint(m_orientation);
        }
    }

    if(item.type == Item::Type::UserSpacer || item.type == Item::Type::StandardSpacer)
    {
        m_spacers.add(item.component);
    }
    else if(item.type == Item::Type::ResizeBar)
    {
        m_resizeBars.add(item.component);
    }

    m_managed->addAndMakeVisible(item.component);

    if(index == -1)
    {
        index = m_items.size();
    }

    m_items.insert(index, item);

    if(item.type == Item::Type::Component)
    {
        item.component->addComponentListener(this);
    }

    std::vector<std::array<double, 3>> sizes((size_t)m_items.size());

    // First read through and read the sizes of the existing widgets, store
    // them to an array, inserting the new widget in the correct spot, and
    // then loop back through recreating the layout.

    // This is required since Juce's layout manager doesn't include support
    // for removing or reordering components.

    for(size_t i = 0; i < (size_t)m_items.size(); i++)
    {
        if(i == (size_t)index)
        {
            sizes[i][0] = constraints.m_minimum;
            sizes[i][1] = constraints.m_preferred;
            sizes[i][2] = constraints.m_maximum;
        }
        else
        {
            size_t j = i > (size_t)index ? i - 1 : i;
            m_layout.getItemLayout((int)j, sizes[i][0], sizes[i][1], sizes[i][2]);
        }
    }

    m_layout.clearAllItems();

    for(size_t i = 0; i < (size_t)m_items.size(); i++)
    {
        m_layout.setItemLayout((int)i, sizes[i][0], sizes[i][1], sizes[i][2]);
    }

    resized();
}

void LayoutManager::insertItemWithSpacer(const Item &item, const Constraints &constraints,
                                         int index)
{
    if(index == -1)
    {
        index = m_items.size();
    }

    if(index != 0 && m_items.size() > 0)
    {
        insertItem(Item(Item::Type::StandardSpacer, new Spacer(m_spacers.size())),
                   Drawing::StandardSpacerConstraints, index);
        insertItem(item, constraints, index + 1);
    }
    else
    {
        insertItem(item, constraints, index);
    }

    if(index == 0 && m_items.size() > 1)
    {
        insertItem(Item(Item::Type::StandardSpacer, new Spacer(m_spacers.size())),
                   Drawing::StandardSpacerConstraints, 1);
    }
}

void LayoutManager::removeItem(int index)
{
    std::vector<std::array<double, 3>> sizes((size_t)(m_items.size() - 1));

    for(auto i = 0; i < m_items.size(); i++)
    {
        if(i != index)
        {
            size_t j = i > index ? (size_t)(i - 1) : (size_t)i;
            m_layout.getItemLayout(i, sizes[j][0], sizes[j][1], sizes[j][2]);
        }
    }

    m_layout.clearAllItems();
    m_spacers.removeObject(m_items[index].component);
    m_items.remove(index);

    for(size_t i = 0; i < (size_t)m_items.size(); i++)
    {
        m_layout.setItemLayout((int)i, sizes[i][0], sizes[i][1], sizes[i][2]);
    }
}

void LayoutManager::componentBeingDeleted(Component &component)
{
    auto index = m_items.indexOf(Item(Item::Type::Component, &component));

    if(index >= 0)
    {
        removeItem(index);
        LayoutManager::resized();
    }
}

void SizeHint::setSizeHint(LayoutManager::Orientation orientation,
                           const LayoutManager::Constraints &hint)
{
    if(orientation == LayoutManager::Orientation::Horizontal)
    {
        m_horizontal = hint;
    }
    else
    {
        m_vertical = hint;
    }
}

LayoutManager::Constraints SizeHint::getSizeHint(LayoutManager::Orientation orientation) const
{
    return orientation == LayoutManager::Orientation::Horizontal ? m_horizontal : m_vertical;
}
