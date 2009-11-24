#ifndef BANNERFACTORY_H
#define BANNERFACTORY_H

class banner;
class QDomNode;

// The factory method that build the banner identified by the configuration node,
// 0 if it can't build the banner
banner *getBanner(const QDomNode &config_node);

#endif // BANNERFACTORY_H
