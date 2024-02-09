// File:        imglist.cpp
// Date:        2022-01-27 10:21
// Description: Contains partial implementation of ImgList class
//              for CPSC 221 2021W2 PA1
//              Function bodies to be completed by yourselves
//
// ADD YOUR FUNCTION IMPLEMENTATIONS IN THIS FILE
//

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/**************************
* MISCELLANEOUS FUNCTIONS *
**************************/

/*
* This function is NOT part of the ImgList class,
* but will be useful for one of the ImgList functions.
* Returns the "difference" between two hue values.
* PRE: hue1 is a double between [0,360).
* PRE: hue2 is a double between [0,360).
* 
* The hue difference is the absolute difference between two hues,
* but takes into account differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
double HueDiff(double hue1, double hue2)
{
  return fmin(fabs(hue1 - hue2), fabs(360 + fmin(hue1, hue2) - fmax(hue1, hue2)));
}

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/*
* Default constructor. Makes an empty list
*/
ImgList::ImgList()
{
  // set appropriate values for all member attributes here
  northwest = NULL;
  southeast = NULL;
}

/*
* Creates a list from image data
* PRE: img has dimensions of at least 1x1
*/
ImgList::ImgList(PNG &img)
{
  ImgList();

  int arrWidth = img.width();
  int arrHeight = img.height();
  ImgNode* arr[arrHeight][arrWidth];

  for (unsigned x = 0; x < img.width(); x++)
  {
    for (unsigned y = 0; y < img.height(); y++)
    {
      ImgNode *newNode = new ImgNode();
      HSLAPixel *pixel = img.getPixel(x, y);
      newNode->colour.h = pixel->h;
      newNode->colour.s = pixel->s;
      newNode->colour.l = pixel->l;
      newNode->colour.a = pixel->a;
      arr[y][x] = newNode;
    }
  }
  cout << "done array storing" << endl;

  for (unsigned x = 0; x < img.width(); x++) {
    for (unsigned y = 0; y < img.height(); y++) {

      if (x == 0 && y == 0) {
        northwest = arr[y][x];
      }
      
      int z;
      // assigns west pointer
      if (0 < x) {
        z = x - 1;
        arr[y][x]->west = arr[y][z];
      }

      // assigns north pointer
      if (y > 0) {
        z = y - 1;
        arr[y][x]->north = arr[z][x];
      }

      // assigns east pointer
      if (x < arrWidth - 1) {
        z = x + 1;
        arr[y][x]->east = arr[y][z];
      }

      // assigns south pointer
      if (y < arrHeight - 1) {
        z = y + 1;
        arr[y][x]->south = arr[z][x];
      }
    }
  }
  southeast = arr[arrHeight-1][arrWidth-1];
  cout << "done array linking" << endl;
}

/*
* Copy constructor.
* Creates this this to become a separate copy of the data in otherlist
*/
ImgList::ImgList(const ImgList &otherlist)
{
  // build the linked node structure using otherlist as a template
  Copy(otherlist);
}

/*
* Assignment operator. Enables statements such as list1 = list2;
*   where list1 and list2 are both variables of ImgList type.
* POST: the contents of this list will be a physically separate copy of rhs
*/
ImgList &ImgList::operator=(const ImgList &rhs)
{
  // Re-build any existing structure using rhs as a template

  if (this != &rhs)
  { // if this list and rhs are different lists in memory
    // release all existing heap memory of this list
    Clear();

    // and then rebuild this list using rhs as a template
    Copy(rhs);
  }

  return *this;
}

/*
* Destructor.
* Releases any heap memory associated with this list.
*/
ImgList::~ImgList()
{
  // Ensure that any existing heap memory is deallocated
  Clear();
}

/************
* ACCESSORS *
************/

/*
* Returns the horizontal dimension of this list (counted in nodes)
* Note that every row will contain the same number of nodes, whether or not
*   the list has been carved.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionX() const
{
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count++;
    curr = curr->east;
  }
  delete curr;
  curr = NULL;
  return count;
}

/*
* Returns the vertical dimension of the list (counted in nodes)
* It is useful to know/assume that the grid will never have nodes removed
*   from the first or last columns. The returned value will thus correspond
*   to the height of the PNG image from which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   y dimension.
*/
unsigned int ImgList::GetDimensionY() const
{
  // replace the following line with your implementation
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count++;
    curr = curr->south;
  }
  delete curr;
  curr = NULL;
  return count;
}

/*
* Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
* The returned value will thus correspond to the width of the PNG image from
*   which this list was constructed.
* We expect your solution to take linear time in the number of nodes in the
*   x dimension.
*/
unsigned int ImgList::GetDimensionFullX() const
{
  // replace the following line with your implementation
  int count = 0;
  ImgNode *curr = northwest;
  while (curr != NULL)
  {
    count++;
    count += curr->skipleft;
    curr = curr->east;
  }
  delete curr;
  curr = NULL;
  return count;
}

/*
* Returns a pointer to the node which best satisfies the specified selection criteria.
* The first and last nodes in the row cannot be returned.
* PRE: rowstart points to a row with at least 3 physical nodes
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rowstart - pointer to the first node in a row
* PARAM: selectionmode - criterion used for choosing the node to return
*          0: minimum luminance across row, not including extreme left or right nodes
*          1: node with minimum total of "hue difference" with its left neighbour and with its right neighbour.
*        In the (likely) case of multiple candidates that best match the criterion,
*        the left-most node satisfying the criterion (excluding the row's starting node)
*        will be returned.
* A note about "hue difference": For PA1, consider the hue value to be a double in the range [0, 360).
* That is, a hue value of exactly 360 should be converted to 0.
* The hue difference is the absolute difference between two hues,
* but be careful about differences spanning the 360 value.
* e.g. Two pixels with hues 90 and 110 differ by 20, but
*      two pixels with hues 5 and 355 differ by 10.
*/
ImgNode *ImgList::SelectNode(ImgNode *rowstart, int selectionmode)
{
  // add your implementation below
  // ImgNode *currNode = rowstart;
  ImgNode *selectedNode = NULL;
  if (selectionmode == 1)
  {
    double minHueDiff = 720;
    while (rowstart->east != NULL && rowstart->west != NULL)
    {
      double leftHue = rowstart->west->colour.h;
      double rightHue = rowstart->east->colour.h;

      double hueDiff = HueDiff(leftHue, rowstart->colour.h) + HueDiff(rightHue, rowstart->colour.h);

      if (minHueDiff > hueDiff)
      {
        minHueDiff = hueDiff;
        selectedNode = rowstart;
      }
      rowstart = rowstart->east;
    }
  } else {
    double minLuminance = 2.0;
    while (rowstart->east != NULL && rowstart->west != NULL) {
      double currLuminance = rowstart->colour.l;
      if (minLuminance > currLuminance) {
        minLuminance = rowstart->colour.l;
        selectedNode = rowstart;
      }
      rowstart = rowstart->east;
    }
  }
  // delete currNode;
  // currNode = NULL;
  return selectedNode;
}

/*
* Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
* PRE: fillmode is an integer in the range of [0,2]
* PARAM: fillgaps - whether or not to fill gaps caused by carving
*          false: render one pixel per node, ignores fillmode
*          true: render the full width of the original image,
*                filling in missing nodes using fillmode
* PARAM: fillmode - specifies how to fill gaps
*          0: solid, uses the same colour as the node at the left of the gap
*          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
*             Note that "average" for hue will use the closer of the angular distances,
*             e.g. average of 10 and 350 will be 0, instead of 180.
*             Average of diametric hue values will use the smaller of the two averages
*             e.g. average of 30 and 210 will be 120, and not 300
*                  average of 170 and 350 will be 80, and not 260
*          2: *** OPTIONAL - FOR BONUS ***
*             linear gradient between the colour (all channels) of the nodes at the left and right of the gap
*             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
*             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
*             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
*             Like fillmode 1, use the smaller difference interval for hue,
*             and the smaller-valued average for diametric hues
*/
PNG ImgList::Render(bool fillgaps, int fillmode) const
{
  PNG outpng; 
  if (fillgaps) {
    outpng.resize(GetDimensionFullX(), GetDimensionY());
  } else {
    outpng.resize(GetDimensionX(), GetDimensionY());
  }

  if (northwest == NULL) {
    return outpng;
  }

  ImgNode* rowHolder = northwest;
  ImgNode* goRight = northwest;
  for (unsigned y = 0; y < outpng.height(); y++) {
    for (unsigned x = 0; x < outpng.width(); x++) {

      if (fillgaps)  {
        if (fillmode == 0) { // fill in gaps with solid colour on left
        HSLAPixel *pixel = outpng.getPixel(x, y);
          pixel->h = goRight->colour.h;
          pixel->s = goRight->colour.s;
          pixel->l = goRight->colour.l;
          pixel->a = goRight->colour.a;
          int toFill = goRight->skipright;
          if (toFill > 0) { 
            while (toFill > 0) {
            x += 1;
            HSLAPixel *pixel = outpng.getPixel(x,y);
            pixel->h = goRight->colour.h;
            pixel->s = goRight->colour.s;
            pixel->l = goRight->colour.l;
            pixel->a = goRight->colour.a;
            toFill--;
            }
          }
          goRight = goRight->east;
          if (x == outpng.width() - 1 || goRight == NULL) {    // if x is larger than png width
              goRight = rowHolder->south; // make the next pixel/node be on the new row
              rowHolder = rowHolder->south;
            }
        } else if (fillmode == 1) { // fill in gaps with average hue values
            int toFill = goRight->skipright;
            HSLAPixel *pixel = outpng.getPixel(x, y);
            pixel->h = goRight->colour.h;
            pixel->s = goRight->colour.s;
            pixel->l = goRight->colour.l;
            pixel->a = goRight->colour.a;
            if (toFill > 0) {
              // double hue1 = goRight->colour.h;
              // double hue2 = goRight->east->colour.h;
              double sat = (goRight->colour.s + goRight->east->colour.s) * 0.5;
              double lum = (goRight->colour.l + goRight->east->colour.l) * 0.5;
              double alp = (goRight->colour.a + goRight->east->colour.a) * 0.5;
              while (toFill > 0) {
              x += 1;
              HSLAPixel *pixel = outpng.getPixel(x,y);
              pixel->h = GetHue(goRight->colour.h, goRight->east->colour.h);
              pixel->s = sat;
              pixel->l = lum;
              pixel->a = alp;
              toFill--;
              }
            }
            goRight = goRight->east;
            if (x == outpng.width() - 1 || goRight == NULL) {    // if x is larger than png width
              goRight = rowHolder->south; // make the next pixel/node be on the new row
              rowHolder = rowHolder->south;
            }
          }
      } else { // not filling in gaps
        int toFill = goRight->skipright;
        HSLAPixel *pixel = outpng.getPixel(x, y);
        pixel->h = goRight->colour.h;
        pixel->s = goRight->colour.s;
        pixel->l = goRight->colour.l;
        pixel->a = goRight->colour.a;
        
        goRight = goRight->east;
        if (x == outpng.width() - 1 || goRight == NULL) {    // if x is larger than png width
          goRight = rowHolder->south; // make the next pixel/node be on the new row
          rowHolder = rowHolder->south;
        }
      }
    }
  }
  return outpng;
}

double ImgList::GetHue(double hue1, double hue2) const {
  // Attempt 3
  double avg1 = (hue1 + hue2) * 0.5;
  double avg2 = avg1 + 180;
  if (avg2 >= 360) {
    avg2 -= 360;
  }

  if (HueDiff(avg1,hue1) < HueDiff(avg2,hue1)) {
    return avg1;
  } else {
    return avg2;
  }
}

/************
* MODIFIERS *
************/

/*
* Removes exactly one node from each row in this list, according to specified criteria.
* The first and last nodes in any row cannot be carved.
* PRE: this list has at least 3 nodes in each row
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: selectionmode - see the documentation for the SelectNode function.
* POST: this list has had one node removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(int selectionmode)
{
  ImgNode *curr = northwest;
  while (curr != NULL) {
    ImgNode *toDelete = SelectNode(curr->east, selectionmode);
    toDelete->west->east = toDelete->east;
    toDelete->west->skipright += 1;

    toDelete->east->west = toDelete->west;
    toDelete->east->skipleft += 1;
    
    if (toDelete->north != NULL ) {
    toDelete->north->south = toDelete->south;
    toDelete->north->skipdown += 1;
    }

    if (toDelete->south != NULL) {
    toDelete->south->north = toDelete->north;
    toDelete->south->skipup += 1;
    }

    delete toDelete;
    toDelete = NULL;
    curr = curr->south;
  }
  delete curr;
  curr = NULL;
}

// note that a node on the boundary will never be selected for removal
/*
* Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
* based on specific selection criteria.
* Note that this should remove one node from every row, repeated "rounds" times,
* and NOT remove "rounds" nodes from one row before processing the next row.
* PRE: selectionmode is an integer in the range [0,1]
* PARAM: rounds - number of nodes to remove from each row
*        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
*        i.e. Ensure that the final list has at least two nodes in each row.
* POST: this list has had "rounds" nodes removed from each row. Neighbours of the created
*       gaps are linked appropriately, and their skip values are updated to reflect
*       the size of the gap.
*/
void ImgList::Carve(unsigned int rounds, int selectionmode) {
  int count;
  if (rounds > GetDimensionX() - 2) {
    count = GetDimensionX() -  2;
  } else {
    count = rounds;
  }

  while (count > 0) {
    Carve(selectionmode);
    count -= 1;
  }
}

/*
* Helper function deallocates all heap memory associated with this list,
* puts this list into an "empty" state. Don't forget to set your member attributes!
* POST: this list has no currently allocated nor leaking heap memory,
*       member attributes have values consistent with an empty list.
*/
void ImgList::Clear()
{

  if (northwest != NULL) {
    ClearRow(northwest);
  }

  northwest = NULL;
  southeast = NULL;

}

void ImgList::ClearRow(ImgNode* curr) {
  ImgNode* nextCurr = curr->south;
  
  ImgNode* deleteCurr = curr;
  ImgNode* goRight = curr;
  while (deleteCurr != NULL) {
    if (deleteCurr->south != NULL) {
      deleteCurr->south->north = NULL;
      deleteCurr->south = NULL;
    }

    if (deleteCurr->east != NULL) {
      deleteCurr->east->west = NULL;
      deleteCurr->east = NULL;
    }
    goRight = goRight->east;
    delete deleteCurr;
    deleteCurr = NULL;
    
  }

  if (nextCurr != NULL) {
    ClearRow(nextCurr);
  }

}

/* ************************
*  * OPTIONAL - FOR BONUS *
** ************************
* Helper function copies the contents of otherlist and sets this list's attributes appropriately
* PRE: this list is empty
* PARAM: otherlist - list whose contents will be copied
* POST: this list has contents copied from by physically separate from otherlist
*/
void ImgList::Copy(const ImgList &otherlist)
{
  // add your implementation here
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/
