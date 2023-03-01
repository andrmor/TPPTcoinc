#ifndef enums_h
#define enums_h

enum class FinderMethods {Basic = 1, Advanced = 2};
// Basic    - no energy splitting allowed, strict multiple rejection
// Advanced - energy is allowed to be split within the same assembly, flexible multiple rejection

enum class RejectionMethods {None = 1, All = 2, EnergyWindow = 3};
// None        - multiples are allowed, the coincidence pair takes two strongest depositions
// All         - all coincidences with the multiplicities larger than two are discarded
// EnergyWndow - multiple coincidence is discarded only if there are more than two energy-accepted depositions

#endif // enums_h
