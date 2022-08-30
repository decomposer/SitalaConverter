#ifndef LAYOUTMANAGER_H_INCLUDED
#define LAYOUTMANAGER_H_INCLUDED

#include <JuceHeader.h>

class StrechableLayoutResizeBar;

/*!
 * This is a wrapper around Juce's StretchableLayoutManager that add as saner
 * and more Qt-like API.
 */

class LayoutManager : private ComponentListener
{
public:
    enum class Orientation
    {
        Horizontal,
        Vertical
    };

    enum class Unit
    {
        Absolute,
        Percentage,
    };

    enum class WithSpacer
    {
        Standard,
        None
    };

    class Constraints
    {
        friend class LayoutManager;
        friend class LayoutManagedComponent;

    public:
        enum class Type
        {
            Minimum,
            Preferred,
            Maximum
        };

        /*!
         * Creates a default set of constraints (no minimum, preferred and maximum
         * size as 100%).  This can be chained with the setters, e.g.:
         *
         * Constraints().setMinimum(Absolute, 100).setMaximum(Absolute, 200);
         */
        Constraints();

        Constraints &setMinimum(Unit unit, float value);
        Constraints &setPreferred(Unit unit, float value);
        Constraints &setMaximum(Unit unit, float value);

        float getMinimum(Unit *unit = nullptr) const;
        float getPreferred(Unit *unit = nullptr) const;
        float getMaximum(Unit *unit = nullptr) const;

        /*!
         *
         */
        Constraints &set(Type type, Unit unit, float value);

        /*!
         * Creates a set of constraints where the minimum, preferred and maximum
         * are all an absolute size of \a value.
         */
        static Constraints fixed(float value);

        /*!
         * Creates a set of constraints where the minimum, preferred and maximum
         * are all an percentage size of \a value.
         */
        static Constraints percentage(float value);

        bool operator==(const Constraints &other) const;

    private:
        float m_minimum;
        float m_preferred;
        float m_maximum;
    };

    LayoutManager(Component *managed, Orientation orientation);
    ~LayoutManager() override;

    virtual void resized();

    void prependComponent(Component *component, const Constraints &constraints = Constraints(),
                          WithSpacer withSpacer = WithSpacer::Standard);
    void prependComponent(Component *component, WithSpacer withSpacer);

    void appendComponent(Component *component, const Constraints &constraints = Constraints(),
                         WithSpacer withSpacer = WithSpacer::Standard);
    void appendComponent(Component *component, WithSpacer withSpacer);

    void insertComponentBefore(Component *before, Component *component,
                              const Constraints &constraints = Constraints(),
                              WithSpacer withSpacer = WithSpacer::Standard);
    void insertComponentBefore(Component *before, Component *component, WithSpacer withSpacer);

    void insertComponentAfter(Component *after, Component *component,
                              const Constraints &constraints = Constraints(),
                              WithSpacer withSpacer = WithSpacer::Standard);
    void insertComponentAfter(Component *after, Component *component, WithSpacer withSpacer);

    Component *removeComponent(Component *component);

    void clear();

    void addSpacer(const Constraints &constraints = Constraints());
    void setBorderSizes(const BorderSize<int> &borders);
    BorderSize<int> getBorderSizes() const;
    void setConstraints(Component *component, const Constraints &constraints);

    void addResizeBar();

    Orientation getOrientation() const
    {
        return m_orientation;
    }

private:
    struct Item
    {
        enum class Type
        {
            Component,
            UserSpacer,
            StandardSpacer,
            ResizeBar
        };

        Item() : type(Type::Component), component(nullptr) {}
        Item(Type t, ::Component *c) : type(t), component(c) {}

        bool operator==(const Item &other) const
        {
            return type == other.type && component == other.component;
        }

        Type type;
        Component *component;
    };

    void insertItem(const Item &item, Constraints constraints, int index = -1);
    void insertItemWithSpacer(const Item &item, const Constraints &constraints, int index = -1);
    void removeItem(int index);

    virtual void componentBeingDeleted(Component &component) override;

    Component *m_managed = nullptr;
    Orientation m_orientation = Orientation::Horizontal;
    BorderSize<int> m_borders = { 0, 0, 0, 0 };
    StretchableLayoutManager m_layout;
    Array<Item> m_items;
    OwnedArray<Component> m_spacers;
    OwnedArray<Component> m_resizeBars;
};

class Drawing
{
public:
    static constexpr float ControlHeight = 20;
    static constexpr float DefaultSpace = 5;
    static const LayoutManager::Constraints StandardSpacerConstraints;
    static const LayoutManager::Constraints ControlHeightConstraints;
};

class SizeHint
{
public:
    virtual ~SizeHint() = default;
    virtual void setSizeHint(LayoutManager::Orientation orientation,
                             const LayoutManager::Constraints &hint);
    virtual LayoutManager::Constraints getSizeHint(LayoutManager::Orientation orientation) const;

private:
    LayoutManager::Constraints m_horizontal;
    LayoutManager::Constraints m_vertical;
};

#endif
