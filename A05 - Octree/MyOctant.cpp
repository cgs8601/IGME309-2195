#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;

void MyOctant::Init( void ) {
    m_uID = 0;
    m_uLevel = 0;
    m_uChildren = 0;

    m_fSize = 0.0f;

    m_pMeshMngr = MeshManager::GetInstance();
    m_pEntityMngr = MyEntityManager::GetInstance();

    m_v3Center = vector3( 0.0f );
    m_v3Min = vector3( 0.0f );
    m_v3Max = vector3( 0.0f );

    m_pParent = nullptr;
    for ( int i = 0; i < 8; i++ ) {
        m_pChild[i] = nullptr;
    }

    m_EntityList = std::vector<uint>();

    m_pRoot = nullptr;
    m_lChild = std::vector<MyOctant *>();

}

MyOctant::MyOctant( uint a_nMaxLevel, uint a_nIdealEntityCount ) {
    Init();

    m_uMaxLevel = a_nMaxLevel;
    m_uIdealEntityCount = a_nIdealEntityCount;

    m_uOctantCount = 1;

    m_uID = 0;
    m_uLevel = 0;

    // These are the default values for if there's no entities.
    // Since we're building a cube, we only need one min and one max.
    float minVal = 0.0f;
    float maxVal = 0.0f;

    for ( int entityID = 0; entityID < m_pEntityMngr->GetEntityCount(); entityID++ ) {
        MyEntity *entity = m_pEntityMngr->GetEntity( entityID );
        MyRigidBody *rigidbody = entity->GetRigidBody();
        vector3 minGlobal = rigidbody->GetMinGlobal();
        vector3 maxGlobal = rigidbody->GetMaxGlobal();

        // set the initial min and max
        if ( entityID == 0 ) {
            minVal = minGlobal.x;
            maxVal = maxGlobal.x;
        }

        minVal = std::min( minVal, minGlobal.x );
        minVal = std::min( minVal, minGlobal.y );
        minVal = std::min( minVal, minGlobal.z );

        maxVal = std::max( maxVal, maxGlobal.x );
        maxVal = std::max( maxVal, maxGlobal.y );
        maxVal = std::max( maxVal, maxGlobal.z );
    }

    float center = ( maxVal + minVal ) / 2;
    m_v3Center = vector3( center, center, center );

    m_v3Min = vector3( minVal, minVal, minVal );
    m_v3Max = vector3( maxVal, maxVal, maxVal );

    m_fSize = maxVal - minVal;

    m_EntityList.clear();
    m_pRoot = this;
    m_lChild.clear();

    ConstructTree( a_nMaxLevel );
}

MyOctant::MyOctant( vector3 a_v3Center, float a_fSize ) {
    Init();

    m_uOctantCount++;
    m_uID = m_uOctantCount;

    m_v3Center = a_v3Center;
    m_fSize = a_fSize;

    m_v3Min = a_v3Center - vector3( m_fSize / 2.0f );
    m_v3Max = a_v3Center + vector3( m_fSize / 2.0f );
}

MyOctant::MyOctant( MyOctant const &other ) {
    m_uID = other.m_uID;
    m_uLevel = other.m_uLevel;
    m_uChildren = other.m_uChildren;

    m_fSize = other.m_fSize;

    m_pMeshMngr = MeshManager::GetInstance();
    m_pEntityMngr = MyEntityManager::GetInstance();

    m_v3Center = other.m_v3Center;
    m_v3Min = other.m_v3Min;
    m_v3Max = other.m_v3Max;

    m_pParent = other.m_pParent;
    for ( int i = 0; i < 8; i++ ) {
        m_pChild[i] = other.m_pChild[i];
    }

    m_EntityList = std::vector<uint>( other.m_EntityList );
    m_lChild = std::vector<MyOctant *>( other.m_lChild );

    m_pRoot = other.m_pRoot;

}

MyOctant &MyOctant::operator=( MyOctant const &other ) {
    if ( this != &other ) {
        Release();
        Init();
        MyOctant temp( other );
        Swap( temp );
    }
    return *this;
}

MyOctant::~MyOctant( void ) {
    Release();
}

void MyOctant::Swap( MyOctant &other ) {
    std::swap( m_uID, other.m_uID );
    std::swap( m_uLevel, other.m_uLevel );
    std::swap( m_uChildren, other.m_uChildren );

    std::swap( m_fSize, other.m_fSize );

    m_pMeshMngr = MeshManager::GetInstance();
    m_pEntityMngr = MyEntityManager::GetInstance();

    std::swap( m_v3Center, other.m_v3Center );
    std::swap( m_v3Min, other.m_v3Min );
    std::swap( m_v3Max, other.m_v3Max );

    std::swap( m_pParent, other.m_pParent );
    std::swap( m_pChild, other.m_pChild );

    std::swap( m_EntityList, other.m_EntityList );
    std::swap( m_lChild, other.m_lChild );

    std::swap( m_pRoot, other.m_pRoot );

}

float MyOctant::GetSize( void ) {
    return m_fSize;
}

vector3 MyOctant::GetCenterGlobal( void ) {
    return m_v3Center;
}

vector3 MyOctant::GetMinGlobal( void ) {
    return m_v3Min;
}

vector3 MyOctant::GetMaxGlobal( void ) {
    return m_v3Max;
}

bool MyOctant::IsColliding( uint a_uRBIndex ) {
    // get the appropriate entity from the entity manager,
    MyEntity *entity = m_pEntityMngr->GetEntity( a_uRBIndex );
    MyRigidBody *rigidbody = entity->GetRigidBody();
    vector3 entityMin = rigidbody->GetMinGlobal();
    vector3 entityMax = rigidbody->GetMaxGlobal();

    // perform AABB with entity and octant
    return ( m_v3Min.x <= entityMax.x && m_v3Max.x >= entityMin.x ) &&
           ( m_v3Min.y <= entityMax.y && m_v3Max.y >= entityMin.y ) &&
           ( m_v3Min.z <= entityMax.z && m_v3Max.z >= entityMin.z );

}

void MyOctant::Display( uint a_nIndex, vector3 a_v3Color ) {
    // Note that this displays the children of the targeted node as well
    if ( a_nIndex == m_uID ) {
        Display( a_v3Color );
    } else {
        for ( int i = 0; i < m_uChildren; i++ ) {
            m_pChild[i]->Display( a_nIndex, a_v3Color );
        }
    }
}

void MyOctant::Display( vector3 a_v3Color ) {
    for ( int i = 0; i < m_uChildren; i++ ) {
        m_pChild[i]->Display( a_v3Color );
    }

    m_pMeshMngr->AddWireCubeToRenderList( glm::translate( IDENTITY_M4, m_v3Center )
                                              * glm::scale( vector3( m_fSize ) ),
                                          a_v3Color,
                                          RENDER_WIRE );
}

void MyOctant::DisplayLeafs( vector3 a_v3Color ) {
    for ( int i = 0; i < m_lChild.size(); i++ ) {
        m_lChild.at( i )->Display( a_v3Color );
    }
}

void MyOctant::ClearEntityList( void ) {
    for ( int i = 0; i < m_uChildren; i++ ) {
        m_pChild[i]->ClearEntityList();
    }

    m_EntityList.clear();
}

void MyOctant::Subdivide( void ) {
    // Ensure that we don't subdivide if:
    // - we're already at the max level
    // - have already subdivided (return if so)
    if ( m_uLevel >= m_uMaxLevel || m_uChildren > 0 ) {
        return;
    }

    // Size of the child nodes
    float childSize = m_fSize / 2.0f;
    // Displacement from the parent center of child nodes
    float childDis = childSize / 2.0f;


    // Create the 8 children, of appropriate size, and appropriately displaced
    m_pChild[0] = new MyOctant( m_v3Center + vector3( childDis, childDis, childDis ),
                                childSize );
    m_pChild[1] = new MyOctant( m_v3Center + vector3( -childDis, childDis, childDis ),
                                childSize );
    m_pChild[2] = new MyOctant( m_v3Center + vector3( childDis, -childDis, childDis ),
                                childSize );
    m_pChild[3] = new MyOctant( m_v3Center + vector3( childDis, childDis, -childDis ),
                                childSize );
    m_pChild[4] = new MyOctant( m_v3Center + vector3( -childDis, -childDis, childDis ),
                                childSize );
    m_pChild[5] = new MyOctant( m_v3Center + vector3( -childDis, childDis, -childDis ),
                                childSize );
    m_pChild[6] = new MyOctant( m_v3Center + vector3( childDis, -childDis, -childDis ),
                                childSize );
    m_pChild[7] = new MyOctant( m_v3Center + vector3( -childDis, -childDis, -childDis ),
                                childSize );

    m_uChildren = 8;

    // Setup each of the children, subdivide if they need to.
    for ( int i = 0; i < m_uChildren; i++ ) {
        MyOctant *child = m_pChild[i];

        child->m_pParent = this;
        child->m_pRoot = m_pRoot;
        child->m_uLevel = m_uLevel + 1;

        if ( child->ContainsMoreThan( m_uIdealEntityCount ) ) {
            child->Subdivide();
        }
    }
}

MyOctant *MyOctant::GetChild( uint a_nChild ) {
    // Check if a_nChild is out of bounds, return nullptr if so
    if ( a_nChild < 0 || a_nChild >= m_uChildren ) {
        return nullptr;
    }
    return m_pChild[a_nChild];
}

MyOctant *MyOctant::GetParent( void ) {
    return m_pParent;
}

bool MyOctant::IsLeaf( void ) {
    return m_uChildren == 0;
}

bool MyOctant::ContainsMoreThan( uint a_nEntities ) {
    // Iterate through all the entities, checking for collisions with the octant.
    uint numColliding = 0;
    for ( int entityID = 0; entityID < m_pEntityMngr->GetEntityCount(); entityID++ ) {
        if ( IsColliding( entityID ) ) {
            numColliding++;
            if ( numColliding > a_nEntities ) {
                return true;
            }
        }
    }
    return false;
}

void MyOctant::KillBranches( void ) {
    // Recurse through children, deleting them and resetting the child-specific information of the node
    // Only the node it was called on (as well as the nodes above that node) will survive
    for ( int i = 0; i < m_uChildren; i++ ) {
        m_pChild[i]->KillBranches();
        SafeDelete( m_pChild[i] );
        m_pChild[i] = nullptr;
    }
    m_uChildren = 0;
}

void MyOctant::ConstructTree( uint a_nMaxLevel ) {
    // Make sure this method is only called on root node.
    if ( m_uLevel > 0 ) {
        return;
    }

    // KillBranches and set related values
    KillBranches();
    
    m_uMaxLevel = a_nMaxLevel;

    m_uOctantCount = 1;

    m_EntityList.clear();
    m_lChild.clear();


    // Subdivide if necessary
    if ( ContainsMoreThan( m_uIdealEntityCount ) ) {
        Subdivide();
    }

    // Assign ids to entities
    AssignIDtoEntity();

    // Construct the list of leaf nodes with entities
    ConstructList();
}

void MyOctant::AssignIDtoEntity( void ) {
    // Search until you're in a leaf
    for ( int i = 0; i < m_uChildren; i++ ) {
        m_pChild[i]->AssignIDtoEntity();
    }

    // If in a leaf node, find any entities in the octant and assign them to the octant
    if ( m_uChildren == 0 ) {
        for ( int entityID = 0; entityID < m_pEntityMngr->GetEntityCount(); entityID++ ) {
            if ( IsColliding( entityID ) ) {
                m_pEntityMngr->AddDimension( entityID, m_uID );
                m_EntityList.push_back( entityID );
            }
        }
    }
}

uint MyOctant::GetOctantCount( void ) {
    return m_uOctantCount;
}

void MyOctant::Release( void ) {
    // Only needs to be performed on the root node, will handle deleting the nodes
    if ( m_uLevel == 0 ) {
        KillBranches();
    }

    m_uID = 0;
    m_uLevel = 0;
    m_uChildren = 0;
    m_fSize = 0.0f;

    m_EntityList.clear();
    m_lChild.clear();
}

void MyOctant::ConstructList( void ) {
    // Construts the list of leaf nodes with entities

    // Search until you're in a leaf
    for ( int i = 0; i < m_uChildren; i++ ) {
        m_pChild[i]->ConstructList();
    }

    // If in a leaf node with entities, add it to the root's list of leaf nodes with entities
    if ( m_uChildren == 0 && !m_EntityList.empty() ) {
        m_pRoot->m_lChild.push_back( this );
    }
}