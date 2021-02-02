/*
 * Xournal++
 *
 * Poppler GLib Implementation
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <poppler.h>

#include "pdf/base/XojPdfDocumentInterface.h"

#include "filesystem.h"

class PopplerGlibDocument: public XojPdfDocumentInterface {
public:
    PopplerGlibDocument();
    PopplerGlibDocument(const PopplerGlibDocument& doc);
    virtual ~PopplerGlibDocument();

public:
    virtual void assign(XojPdfDocumentInterface* doc) override;
    virtual bool equals(XojPdfDocumentInterface* doc) override;

public:
    virtual bool save(fs::path const& filepath, GError** error) override;
    virtual bool load(fs::path const& filepath, string password, GError** error) override;
    virtual bool load(gpointer data, gsize length, string password, GError** error) override;
    virtual bool isLoaded() override;

    virtual XojPdfPageSPtr getPage(size_t page) override;

    virtual size_t getPageCount() override;
    virtual XojPdfBookmarkIterator* getContentsIter() override;

private:
    PopplerDocument* document = nullptr;
};
