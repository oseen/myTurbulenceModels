/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
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

Class
    Foam::RASModels::EARSMTrans

Group
    grpRASTurbulence

Description
   Implementation of the EARSM turbulence model coupled to an algebraic transition
   model.

    This turbulence model is described in:
    \verbatim
        Holman, Furst: not yet published
    \endverbatim

    The default model coefficients are
    \verbatim
        EARSMTransCoeffs
        {
            betaStar       0.09
	    alphaK         1.01       // sigma^*
	    alphaOmega     0.5        // sigma
	    beta           0.075
	    sigmaD         0.52
	    gamma          5./9.
	    Ctau           6.0
	    CSS            3.25
	    CT             14.5/8
	    AT             1.0
            productionLimiter false;
        }
    \endverbatim

SourceFiles
    EARSM.C

\*---------------------------------------------------------------------------*/

#ifndef EARSMTrans_H
#define EARSMTrans_H

#include "RASModel.H"
#include "nonlinearEddyViscosity.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace RASModels
{

/*---------------------------------------------------------------------------*\
                           Class EARSMTrans Declaration
\*---------------------------------------------------------------------------*/

template<class BasicTurbulenceModel>
class EARSMTrans
:
    public nonlinearEddyViscosity<RASModel<BasicTurbulenceModel> >
{
    // Private memmber functions
    
    // Disallow default bitwise copy construct and assignment
    EARSMTrans(const EARSMTrans&);
    EARSMTrans& operator=(const EARSMTrans&);
    
    
protected:
    
    // Protected data
    
    // Model coefficients

    dimensionedScalar betaStar_;

    dimensionedScalar alphaK_;

    dimensionedScalar alphaOmega_;

    dimensionedScalar beta_;

    dimensionedScalar sigmaD_;

    dimensionedScalar gamma_;
    
    dimensionedScalar Ctau_;

    dimensionedScalar CSS_;

    dimensionedScalar CT_;
    
    dimensionedScalar AT_;

    Switch productionLimiter_;
    
    // Fields
    
    volScalarField k_;
    volScalarField omega_;

    //- Wall distance
    //  Note: different to wall distance in parent RASModel
    //  which is for near-wall cells only
    const volScalarField& y_;


    // Protected Member Functions


    virtual void correctNut();

    virtual volScalarField N(
        const volScalarField& A3p,
        const volScalarField& P1,
        const volScalarField& P2
    ) const;
    
    virtual void correctNonlinearStress(const volTensorField& gradU);

public:

    typedef typename BasicTurbulenceModel::alphaField alphaField;
    typedef typename BasicTurbulenceModel::rhoField rhoField;
    typedef typename BasicTurbulenceModel::transportModel transportModel;


    //- Runtime type information
    TypeName("EARSMTrans");


    // Constructors

        //- Construct from components
        EARSMTrans
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName,
            const word& type = typeName
        );


    //- Destructor
    virtual ~EARSMTrans()
    {}


    // Member Functions
    
    //- Re-read model coefficients if they have changed
    virtual bool read();
    
    //- Return the effective diffusivity for k
    tmp<volScalarField> DkEff() const
        {
            return tmp<volScalarField>
                (
                    new volScalarField("DkEff", alphaK_*this->nut_ + this->nu())
                );
        }
    
    //- Return the effective diffusivity for omega
    tmp<volScalarField> DomegaEff() const
        {
            return tmp<volScalarField>
                (
                    new volScalarField("DomegaEff", alphaOmega_*this->nut_ + this->nu())
                );
        }
    
    //- Return the total fluctuation kinetic energy
    virtual tmp<volScalarField> k() const
        {
            return k_;
        }
    
    
    //- Return the turbulence specific dissipation rate
    virtual tmp<volScalarField> omega() const
        {
            return omega_;
        }
    
    //- Return the total fluctuation kinetic energy dissipation rate
    virtual tmp<volScalarField> epsilon() const
        {
            return tmp<volScalarField>
                (
                    new volScalarField
                    (
                    IOobject
                    (
                        "epsilon",
                        this->mesh_.time().timeName(),
                        this->mesh_
                    ),
                    betaStar_*k_*omega_,
                    omega_.boundaryField().types()
                    )
                );
            
        }
    
    //- Validate the turbulence fields after construction
    //  Update turbulence viscosity and other derived fields as requires
    virtual void validate();
    
    //- Solve the turbulence equations and correct the turbulence viscosity
    virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#ifdef NoRepository
#   include "EARSMTrans.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
