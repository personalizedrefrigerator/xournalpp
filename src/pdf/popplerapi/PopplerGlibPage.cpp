#include "PopplerGlibPage.h"
#include "PopplerGlibAction.h"

#include <poppler-page.h>


PopplerGlibPage::PopplerGlibPage(PopplerPage* page, PopplerDocument* parentDoc): page(page), document(parentDoc) {
    if (page != nullptr) {
        g_object_ref(page);
    }
}

PopplerGlibPage::PopplerGlibPage(const PopplerGlibPage& other): page(other.page), document(other.document) {
    if (page != nullptr) {
        g_object_ref(page);
    }
}

PopplerGlibPage::~PopplerGlibPage() {
    if (page) {
        g_object_unref(page);
        page = nullptr;
    }

    document = nullptr;
}

PopplerGlibPage& PopplerGlibPage::operator=(const PopplerGlibPage& other) {
    if (&other == this) {
        return *this;
    }
    if (page) {
        g_object_unref(page);
        page = nullptr;
    }

    page = other.page;
    if (page != nullptr) {
        g_object_ref(page);
    }

    document = other.document;

    return *this;
}

auto PopplerGlibPage::getWidth() const -> double {
    double width = 0;
    poppler_page_get_size(const_cast<PopplerPage*>(page), &width, nullptr);

    return width;
}

auto PopplerGlibPage::getHeight() const -> double {
    double height = 0;
    poppler_page_get_size(const_cast<PopplerPage*>(page), nullptr, &height);

    return height;
}

void PopplerGlibPage::render(cairo_t* cr, bool forPrinting)  // NOLINT(google-default-arguments)
{
    if (forPrinting) {
        poppler_page_render_for_printing(page, cr);
    } else {
        poppler_page_render(page, cr);
    }
}

auto PopplerGlibPage::getPageId() -> int { return poppler_page_get_index(page); }

auto PopplerGlibPage::findText(string& text) -> vector<XojPdfRectangle> {
    vector<XojPdfRectangle> findings;

    double height = getHeight();
    GList* matches = poppler_page_find_text(page, text.c_str());

    for (GList* l = matches; l && l->data; l = g_list_next(l)) {
        auto* rect = static_cast<PopplerRectangle*>(l->data);

        findings.emplace_back(rect->x1, height - rect->y1, rect->x2, height - rect->y2);

        poppler_rectangle_free(rect);
    }
    g_list_free(matches);

    return findings;
}

auto PopplerGlibPage::getLinks() -> std::vector<Link> {
    std::vector<Link> results;
    GList* links = poppler_page_get_link_mapping(this->page);
    double height = this->getHeight();

    for (GList* l = links; l != NULL; l = g_list_next(l)) {
        const auto& link = *static_cast<PopplerLinkMapping*>(l->data);

        if (link.action) {
            XojPdfRectangle rect{link.area.x1, height - link.area.y2, link.area.x2, height - link.area.y1};
            results.emplace_back(rect, std::make_unique<PopplerGlibAction>(link.action, document));
        }
    }
    poppler_page_free_link_mapping(links);

    return results;
}
