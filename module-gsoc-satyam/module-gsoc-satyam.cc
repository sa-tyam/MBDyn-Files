/*  
    Hello team,
    I am Satyam, second year Computer Science and Engineering student at National 
    Institute of Technology, Hamirpur, India.
    I am determined to contribute to the MBDyn community and planning to 
    participate in GSOC-2021 by applying for MBDyn community.
    
    For my entry test I planned to use the relation between 
    - strain energy per unit length (Ul)
    - strain (E)
    - force  (F)
    to create this module. 
    
    the mathematical formulae used are 
    
    force (F) = ( 2 * Ul ) / strain (E)

    In order to prepare this module I took references from the 'module-constlaw', 
    'module-cont-contact' and 'module-hunt-crossley' present in the module directory.

*/

#include "mbconfig.h" 

#include <cmath>
#include <cfloat>

#include "dataman.h"
#include "constltp.h"

class StainEnergyLaw
: public ConstitutiveLaw <doublereal, doublereal> {
    
    private:
        doublereal dStrain_epl;              /* strain energy per unit length */

    public:
        StrainEnergyLaw(doublereal sepl)
        : dStrain_epl(sepl)
        {
            ConstitutiveLaw<doublereal, doublereal>::FDE = mb_deye<doublereal>(dStrain_epl);
        };

        virtual ~StrainEnergyLaw(void)
        {
            NO_OP;
        };

        ConstLawType::Type GetConstLawType(void) const
        {
            return ConstLawType::ELASTIC;
        };

        virtual ConstitutiveLaw<doublereal, doublereal> *pCopy(void) const
        {
            ConstitutiveLaw<doublereal, doublereal> *pCL = NULL;
        
            SAFENEWWITHCONSTRUCTOR(pCL, StrainEnergyLaw, StrainEnergyLaw(dStrain_epl));
            return pCL;
        };

        virtual std::ostream &Restart(std::ostream &out) const
        {
            return out << "Strain Energy per Unit Length, " << dStrain_epl;
        };

        virtual void Update(const doublereal &Eps, const doublereal & /* EpsPrime */ = 0.)
        {
            ConstitutiveLaw<doublereal, doublereal>::Epsilon = Eps;
            ConstitutiveLaw<doublereal, doublereal>::F = (2 * dStrain_epl) / ConstitutiveLaw<doublereal, doublereal>::Epsilon ;
        };

};

/* specific functional object(s) */
struct StrainEnergyLawCLR : public ConstitutiveLawRead<doublereal, doublereal>
{
    virtual ConstitutiveLaw<doublereal, doublereal> *
    Read(const DataManager *pDM, MBDynParser &HP, ConstLawType::Type &CLType) {

        ConstitutiveLaw<doublereal, doublereal> *pCL = 0;

        CLType = ConstLawType::ELASTIC;

        doublereal dS = HP.GetReal();
        if (dS <= 0.)
        {
            silent_cerr("warning, null or negative strain energy per unit length "
                        "at line "
                        << HP.GetLineData() << std::endl);
        }

        SAFENEWWITHCONSTRUCTOR(pCL, StrainEnergyLaw, StrainEnergyLaw(dS));

        return pCL;
    };
};



extern "C" int
module_init(const char *module_name, void *pdm, void *php)
{
#if 0
	DataManager	*pDM = (DataManager *)pdm;
	MBDynParser	*pHP = (MBDynParser *)php;
#endif

    ConstitutiveLawRead<doublereal, doublereal> *rf1D = new StrainEnergyLawCLR;
    if (!SetCL1D("strain" "energy", rf1D))
    {
        delete rf1D;

        silent_cerr("StrainEnergyLaw1D: "
                    "module_init("
                    << module_name << ") "
                                      "failed"
                    << std::endl);

        return -1;
    }

    ConstitutiveLawRead<Vec3, Mat3x3> *rf3D = new StrainEnergyLawCLR<Vec3, Mat3x3>;
    if (!SetCL3D("strain" "energy", rf3D))
    {
        delete rf3D;

        silent_cerr("StrainEnergyLaw3D: "
                    "module_init("
                    << module_name << ") "
                                      "failed"
                    << std::endl);

        return -1;
    }

    ConstitutiveLawRead<Vec6, Mat6x6> *rf6D = new StrainEnergyLawCLR<Vec6, Mat6x6>;
    if (!SetCL6D("strain" "energy", rf6D))
    {
        delete rf6D;
        silent_cerr("StrainEnergyLaw6D: "
                    "module_init("
                    << module_name << ") "
                                      "failed"
                    << std::endl);
        return -1;
    }

    return 0;
}
