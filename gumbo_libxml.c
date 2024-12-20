// Copyright 2015 Jonathan Tang (jonathan.d.tang@gmail.com). All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gumbo_libxml.h"

#include <assert.h>
#include <string.h>

#include "gumbo.h"
#include "libxml/HTMLtree.h"
#include "libxml/tree.h"

// Namespace constants, indexed by GumboNamespaceEnum.
static const char* kLegalXmlns[] = {
  "http://www.w3.org/1999/xhtml",
  "http://www.w3.org/2000/svg",
  "http://www.w3.org/1998/Math/MathML"
};

static xmlNodePtr convert_node(
  xmlDocPtr doc, GumboNode* node) {
  xmlNodePtr result;
  switch (node->type) {
    case GUMBO_NODE_DOCUMENT:
      assert(false &&
        "convert_node cannot be used on the document node.  "
        "Doctype information is automatically added to the xmlDocPtr.");
      break;
    case GUMBO_NODE_ELEMENT:
    case GUMBO_NODE_TEMPLATE:
      {
        GumboElement* elem = &node->v.element;
        // Tag name & namespace.
        xmlNsPtr namespace = NULL;

        const char *elementName = gumbo_normalized_tagname(elem->tag);
        if (strlen(elementName) > 0) {
            result = xmlNewNode(NULL, BAD_CAST elementName);
        } else {
            GumboStringPiece gsp = elem->original_tag;
            gumbo_tag_from_original_text(&gsp);
            xmlChar *unknownTagName = xmlCharStrndup(gsp.data, gsp.length);
            result = xmlNewNode(NULL, unknownTagName);
            xmlFree(unknownTagName);
        }

        if (node->parent->type != GUMBO_NODE_DOCUMENT &&
            elem->tag_namespace != node->parent->v.element.tag_namespace) {
          namespace = xmlNewNs(
              result, BAD_CAST kLegalXmlns[elem->tag_namespace], NULL);
          xmlSetNs(result, namespace);
        }

        // Attributes.
        for (int i = 0; i < elem->attributes.length; ++i) {
          GumboAttribute* attr = elem->attributes.data[i];
          xmlNewProp(result, BAD_CAST attr->name, BAD_CAST attr->value);
        }

        // Children.
        for (int i = 0; i < elem->children.length; ++i) {
          xmlAddChild(result, convert_node(
              doc, elem->children.data[i]));
        }
      }
      break;
    case GUMBO_NODE_TEXT:
    case GUMBO_NODE_WHITESPACE:
      result = xmlNewText(BAD_CAST node->v.text.text);
      break;
    case GUMBO_NODE_COMMENT:
      result = xmlNewComment(BAD_CAST node->v.text.text);
      break;
    case GUMBO_NODE_CDATA:
      {
        // TODO: probably would be faster to use some calculation on
        // original_text.length rather than strlen, but I haven't verified that
        // that's correct in all cases.
        const char* node_text = node->v.text.text;
        result = xmlNewCDataBlock(doc, BAD_CAST node_text, strlen(node_text));
      }
      break;
    default:
      assert(false && "unknown node type");
  }
  return result;
}

xmlDocPtr gumbo_libxml_parse_with_options(
  GumboOptions* options, const char* buffer, size_t buffer_length) {
  GumboOutput* output = gumbo_parse_with_options(options, buffer, buffer_length);
  GumboDocument* doctype = & output->document->v.document;
  const char* system_id = doctype->system_identifier;
  const char* public_id = doctype->public_identifier;
  xmlDocPtr doc;

  if (system_id && !*system_id) system_id = NULL;
  if (public_id && !*public_id) public_id = NULL;
  doc = htmlNewDoc(BAD_CAST system_id, BAD_CAST public_id);

  GumboVector* children = &output->document->v.element.children;
  for (unsigned int i = 0; i < children->length; i++) {
    GumboNode* child = (GumboNode*) children->data[i];

    switch (child->type) {
      case GUMBO_NODE_COMMENT:
        xmlAddChild((xmlNodePtr) doc, xmlNewDocComment(doc, BAD_CAST child->v.text.text));
        break;
      case GUMBO_NODE_ELEMENT:
        xmlDocSetRootElement(doc, convert_node(doc, output->root));
        break;
      default:
        break;
    }
  }

  gumbo_destroy_output(options, output);
  return doc;

}

xmlDocPtr gumbo_libxml_parse(const char* buffer) {
  GumboOptions options = kGumboDefaultOptions;
  return gumbo_libxml_parse_with_options(&options, buffer, strlen(buffer));
}
