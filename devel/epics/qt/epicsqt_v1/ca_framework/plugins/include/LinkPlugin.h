/*! 
  \class LinkPlugin
  \version $Revision: #6 $
  \date $DateTime: 2010/09/06 11:58:56 $
  \author andrew.rhyder
  \brief Link Widget Plugin for designer.
 */
/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef LINKPLUGIN_H
#define LINKPLUGIN_H

#include <QString>
#include <Link.h>

class LinkPlugin : public Link {
    Q_OBJECT

  public:
    /// Constructors
    LinkPlugin( QWidget *parent = 0 );

    /// Qt Designer Properties - condition
    Q_ENUMS(ConditionNames)
    Q_PROPERTY(ConditionNames condition READ getConditionProperty WRITE setConditionProperty)
    enum ConditionNames { Equal              = Link::CONDITION_EQ,
                          NotEqual           = Link::CONDITION_NE,
                          GreaterThan        = Link::CONDITION_GT,
                          GreaterThanOrEqual = Link::CONDITION_GE,
                          LessThan           = Link::CONDITION_LT,
                          LessThanOrEqual    = Link::CONDITION_LE };
    void setConditionProperty( ConditionNames condition ){ setCondition( (Link::conditions)condition ); }
    ConditionNames getConditionProperty(){ return (ConditionNames)getCondition(); }

    Q_PROPERTY(QString comparisonValue READ getComparisonValue WRITE setComparisonValue)
    Q_PROPERTY(bool signalTrue READ getSignalTrue WRITE setSignalTrue)
    Q_PROPERTY(bool signalFalse READ getSignalFalse WRITE setSignalFalse)
    Q_PROPERTY(QString outTrueValue READ getOutTrueValue WRITE setOutTrueValue)
    Q_PROPERTY(QString outFalseValue READ getOutFalseValue WRITE setOutFalseValue)
};

#endif /// LINKPLUGIN_H
