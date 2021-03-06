/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "simpleControl.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(simpleControl, 0);
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::simpleControl::read()
{
    solutionControl::read(true);
}


bool Foam::simpleControl::criteriaSatisfied()
{
    if (residualControl_.empty())
    {
        return false;
    }

    bool achieved = true;
    const dictionary& solverDict = mesh_.solverPerformanceDict();

    forAllConstIter(dictionary, solverDict, iter)
    {
        const word& variableName = iter().keyword();
        label fieldI = applyToField(variableName);
        if (fieldI != -1)
        {
            const List<lduMatrix::solverPerformance> sp(iter().stream());
            const scalar residual = sp.first().initialResidual();

            bool absCheck = residual < residualControl_[fieldI].absTol;
            achieved = achieved && absCheck;

            if (debug)
            {
                Info<< algorithmName_ << " solution statistics:" << endl;

                Info<< "    " << variableName << ": tolerance = " << residual
                    << " (" << residualControl_[fieldI].absTol << ")"
                    << endl;
            }
        }
    }

    return achieved;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::simpleControl::simpleControl(fvMesh& mesh)
:
    solutionControl(mesh, "SIMPLE"),
    initialised_(false)
{
    read();

    Info<< nl;

    if (residualControl_.size() > 0)
    {
        Info<< algorithmName_ << ": convergence criteria" << nl;
        forAll(residualControl_, i)
        {
            Info<< "    field " << residualControl_[i].name << token::TAB
                << " tolerance " << residualControl_[i].absTol
                << nl;
        }
        Info<< endl;
    }
    else
    {
        Info<< algorithmName_ << ": no convergence criteria found. "
            << "Calculations will run for " << mesh_.time().endTime().value()
            << " steps." << nl << endl;
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::simpleControl::~simpleControl()
{}


// ************************************************************************* //
