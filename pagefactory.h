#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

class Page;

// The factory method that build the page related to id if exists and return it, otherwise return 0
Page *getPage(int id);

#endif // PAGEFACTORY_H
