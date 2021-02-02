#include "LinkDestination.h"

#include <utility>

#include "Util.h"

struct _LinkDestClass {
    GObjectClass base_class;
};

G_DEFINE_TYPE(XojLinkDest, link_dest, G_TYPE_OBJECT)  // @suppress("Unused static function")

static void link_dest_init(XojLinkDest* linkAction) { linkAction->dest = nullptr; }

static gpointer parent_class = nullptr;

static void link_dest_finalize(GObject* object) {
    delete LINK_DEST(object)->dest;
    LINK_DEST(object)->dest = nullptr;

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void link_dest_dispose(GObject* object) { G_OBJECT_CLASS(parent_class)->dispose(object); }

static void link_dest_class_init(XojLinkDestClass* linkClass) {
    GObjectClass* g_object_class = nullptr;

    parent_class = g_type_class_peek_parent(linkClass);

    g_object_class = G_OBJECT_CLASS(linkClass);

    g_object_class->dispose = link_dest_dispose;
    g_object_class->finalize = link_dest_finalize;
}

auto link_dest_new() -> XojLinkDest* { return LINK_DEST(g_object_new(TYPE_LINK_DEST, nullptr)); }

LinkDestination::LinkDestination():
        _page(npos),
        _changeLeft(false),
        _changeZoom(false),
        _changeTop(false),
        _zoom(0),
        _left(0),
        _top(0),
        _expand(false),
        _isURI(false),
        _name(""),
        _uri("") {}

LinkDestination::~LinkDestination() = default;

auto LinkDestination::getPdfPage() const -> size_t { return _page; }

void LinkDestination::setPdfPage(size_t page) { _page = page; }

void LinkDestination::setExpand(bool expand) { _expand = expand; }

auto LinkDestination::getExpand() const -> bool { return _expand; }

auto LinkDestination::shouldChangeLeft() const -> bool { return _changeLeft; }

auto LinkDestination::shouldChangeTop() const -> bool { return _changeTop; }

auto LinkDestination::getZoom() const -> double { return _zoom; }

auto LinkDestination::getLeft() const -> double { return _left; }

auto LinkDestination::getTop() const -> double { return _top; }

void LinkDestination::setChangeLeft(double left) {
    _left = left;
    _changeLeft = true;
}

void LinkDestination::setChangeZoom(double zoom) {
    _zoom = zoom;
    _changeZoom = true;
}

void LinkDestination::setChangeTop(double top) {
    _top = top;
    _changeTop = true;
}

void LinkDestination::setName(string name) { _name = std::move(name); }
auto LinkDestination::getName() -> string { return _name; }

void LinkDestination::setURI(string uri) {
    _uri = std::move(uri);
    _isURI = true;
}
auto LinkDestination::getURI() const -> string { return _uri; }
bool LinkDestination::isURI() const { return _isURI; }
