#include "StdAfx.h"
#include "GameApp.h"
#include <D3dx9math.h>
#include "StdAfx.h"
#include <iostream>
#include <string>
#pragma warning( disable : 4996 )
using namespace std;
extern HWND g_hWnd;

CGameApp::CGameApp(void)
{
	m_nGameState = INIT;
}


CGameApp::~CGameApp(void)
{
}

BOOL CGameApp::CheckCubeIntersection(D3DXVECTOR3* vMin1, D3DXVECTOR3* vMax1, D3DXVECTOR3* vMin2, D3DXVECTOR3* vMax2)
{
	if (vMin1->x <= vMax2->x && vMax1->x >= vMin2->x &&
		vMin1->y <= vMax2->y && vMax1->y >= vMin2->y &&
		vMin1->z <= vMax2->z && vMax1->z >= vMin2->z)
		return TRUE;

	return FALSE;
}


void CGameApp::OnInit()
{
	int i;

	RECT rect;
	D3DVIEWPORT9 vp;
	switch (m_nGameState)
	{
	case INIT:
		GetClientRect(m_hWnd, &rect);

		vp.X = 0;
		vp.Y = 0;
		vp.Width = rect.right - rect.left;  // ��ǥ������ �ƴ϶� ũ�� ���� 800, 600 ����
		vp.Height = rect.bottom - rect.top;
		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;

		m_Eye.x = 0.0f;
		m_Eye.y = 0.0f;
		m_Eye.z = -12.0f;

		m_At.x = 0.0f;
		m_At.y = 0.0f;
		m_At.z = 0.0f;

		m_Up.x = 0.0f;
		m_Up.y = 1.0f;
		m_Up.z = 0.0f;

		D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &m_Up);
		m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

		D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
		m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
		m_pd3dDevice->SetViewport(&vp);

		// Note: �޽� ����
		D3DXCreateSphere(m_pd3dDevice, 0.2f, 20, 10, &m_pMesh, NULL);
		D3DXCreateCylinder(m_pd3dDevice, 1.0f, 1.0f, 5.0f, 15, 10, &m_pPlayerBulletMesh, NULL);
		D3DXCreateBox(m_pd3dDevice, 1.0f, 1.0f, 1.0f, &m_pEnemyBoxMesh, NULL);
		D3DXCreateSphere(m_pd3dDevice, 0.2f, 20, 10, &m_pEnemyBulletMesh, NULL);
		m_Ground.Create(m_pd3dDevice, 20, 7, 2.0f);

		// ��Ʈ
		D3DXCreateFont(m_pd3dDevice, 50, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			"Arial", &m_pFont);

		D3DXCreateFont(m_pd3dDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			"Arial", &m_pFont2);
		D3DXCreateFont(m_pd3dDevice, 20, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			"Arial", &m_pFont3);

		
		m_dwGameStartTime = GetTickCount();
		// ���ΰ��� �ִ� �ּ� ���� �� <= �� ��ǥ
		D3DXVECTOR3 *pVertices;
		m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
		D3DXComputeBoundingBox(pVertices, m_pMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pMesh->GetFVF()), &m_sPlayer.vMin, &m_sPlayer.vMax);
		m_pMesh->UnlockVertexBuffer();


		// ���ΰ� �̻��� �ִ� �ּ����� ���ϱ�
		m_pPlayerBulletMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
		D3DXComputeBoundingBox(pVertices, m_pPlayerBulletMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pPlayerBulletMesh->GetFVF()), &m_sPlayerBulletProperty.vMin, &m_sPlayerBulletProperty.vMax);
		m_pPlayerBulletMesh->UnlockVertexBuffer();

		// �� �浹 ��ǥ 	
		m_pEnemyBoxMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
		D3DXComputeBoundingBox(pVertices, m_pEnemyBoxMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pEnemyBoxMesh->GetFVF()), &m_EnemyProperty.vMin, &m_EnemyProperty.vMax);
		m_pEnemyBoxMesh->UnlockVertexBuffer();

		// �� �Ѿ� 
		m_pEnemyBulletMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
		D3DXComputeBoundingBox(pVertices, m_pEnemyBoxMesh->GetNumVertices(), D3DXGetFVFVertexSize(m_pEnemyBulletMesh->GetFVF()), &m_EnemyBulletProperty.vMin, &m_EnemyBulletProperty.vMax);
		m_pEnemyBulletMesh->UnlockVertexBuffer();
		break;
	case READY:
		// ���� ��� ����
		m_Eye.x = 0.0f;
		m_Eye.y = 10.0f;
		m_Eye.z = -35.0f;

		m_At.x = 0.0f;
		m_At.y = 0.0f;
		m_At.z = 0.0f;

		m_Up.x = 0.0f;
		m_Up.y = 1.2f;
		m_Up.z = 0.0f;

		D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &m_Up);
		m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

		// Note: ���ΰ� ����
		m_sPlayer.nLife = 5;
		m_sPlayer.fScale = 3.5f;
		m_sPlayer.fRotationY = -D3DXToRadian(90);
		m_sPlayer.vPos = D3DXVECTOR3(0.0f, 0.0f, -9 * 2.0f);
		m_sPlayer.fVelocity = 0.005f;
		m_sPlayer.dwBulletFireTime = 100;
		m_sPlayer.dwOldBulletFireTime = GetTickCount();
		for (i = 0; i < 10; i++)
			m_sPlayerBullet[i].nLife = 0;

		D3DXMatrixScaling(&m_sPlayer.matScale, m_sPlayer.fScale, m_sPlayer.fScale, m_sPlayer.fScale);
		D3DXMatrixTranslation(&m_sPlayer.matTranslation, m_sPlayer.vPos.x, m_sPlayer.vPos.y, m_sPlayer.vPos.z);
		D3DXMatrixRotationY(&m_sPlayer.matRotationY, m_sPlayer.fRotationY);
		
		// Note: ���ΰ� �̻��� ����
		m_sPlayerBulletProperty.fBulletVelcoty = 0.1f;
		m_sPlayerBulletProperty.fScale = 0.2f;
		D3DXMatrixScaling(&m_sPlayerBulletProperty.matScale, m_sPlayerBulletProperty.fScale, m_sPlayerBulletProperty.fScale, m_sPlayerBulletProperty.fScale);

		// Note: �� ĳ���� ����
		m_EnemyProperty.fScale = 1.0f;
		m_EnemyProperty.fMoveVelcoty = 0.006f;
		m_EnemyProperty.dwFireTime = 1200;
		D3DXMatrixScaling(&m_EnemyProperty.matScale, m_EnemyProperty.fScale, m_EnemyProperty.fScale, m_EnemyProperty.fScale);

		int nRandomNum;
		int nRandomNum2;
		for (i = 0; i < 100; i++)
		{
			m_Enemy[i].nLife = 0;
			m_Enemy[i].dwAppearTime = 2000 + (i * 800);
			m_Enemy[i].dwOldFireTime = GetTickCount();
			nRandomNum = rand() % 5;
			if (nRandomNum % 2)
				m_Enemy[i].vPos.x = -1.0f * nRandomNum;
			else
				m_Enemy[i].vPos.x = (float)nRandomNum;
			nRandomNum2 = rand() % 9;
			m_Enemy[i].vPos.y = (float)nRandomNum+1.0f;
			m_Enemy[i].vPos.z = 20.0f;

			D3DXMatrixTranslation(&m_Enemy[i].matTranslation, m_Enemy[i].vPos.x, m_Enemy[i].vPos.y, m_Enemy[i].vPos.z);
		}

		// Note: �� �̻��� ����
		m_EnemyBulletProperty.fBulletVelcoty = 0.01f;
		m_EnemyBulletProperty.fScale = 0.9f;
		D3DXMatrixScaling(&m_EnemyBulletProperty.matScale, m_EnemyBulletProperty.fScale, m_EnemyBulletProperty.fScale, m_EnemyBulletProperty.fScale);
		for (i = 0; i < 100; i++)
			m_EnemyBullet[i].nLife = 0;


		m_dwGameStartTime = GetTickCount();
		m_nGrade = 0; // ���� �ʱ�ȭ
		
		break;
	}

}

void CGameApp::OnRender()
{
	int i;
	D3DXMATRIX matWorld, matScale, matTrans, matRotation;
	RECT rt;
	char string[100];

	switch (m_nGameState)
	{
	case INIT:
	/*	D3DXMatrixScaling(&matScale, 2.0f, 2.0f, 2.0f);
		D3DXMatrixTranslation(&matTrans, 0.0f, -1.0f, 0.0f);
		D3DXMatrixRotationY(&matRotation, GetTickCount() * 0.001f);

		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		matWorld = matScale * matRotation * matTrans;
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		m_pMesh->DrawSubset(0);

		D3DXMatrixScaling(&matScale, 1.0f, 1.0f, 1.0f);
		D3DXMatrixTranslation(&matTrans, 3.0f, 1.0f, 0.0f);
		D3DXMatrixRotationX(&matRotation, GetTickCount() * 0.002f);
		matWorld = matScale * matRotation * matTrans;
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		m_pPlayerBulletMesh->DrawSubset(0);

		SetRect(&rt, 220, 130, 0, 0);
		SetRect(&rt, 250, 500, 0, 0);*/
		m_pFont2->DrawText(NULL, "Space Ű�� ���� �ּ��� !!", -1, &rt, DT_NOCLIP,
			D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		break;

	case READY:
		SetRect(&rt, 300, 250, 0, 0);
		sprintf(string, "Stage %d", m_nStage);
		m_pFont->DrawText(NULL, string, -1, &rt, DT_NOCLIP,
			D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		m_nEnemyCount = 100;
		break;

	case RUN:
		m_Ground.OnRender();

		// ���ΰ� �Ѿ�
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		for (i = 0; i < 10; i++)
		{
			if (m_sPlayerBullet[i].nLife > 0)
			{
				matWorld = m_sPlayerBulletProperty.matScale * m_sPlayerBullet[i].matTranslation;
				m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
				m_pPlayerBulletMesh->DrawSubset(0);
			}
		}

		// �� �Ѿ� 
		for (i = 0; i < 100; i++)
		{
			if (m_EnemyBullet[i].nLife > 0)
			{
				matWorld = m_EnemyBullet[i].matTranslation;
				m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
				m_pEnemyBulletMesh->DrawSubset(0);
			}
		}

		// �� ĳ����		
		for (i = 0; i < m_nEnemyIndex; i++)
		{
			if (m_Enemy[i].nLife)
			{
				matWorld = m_EnemyProperty.matScale * m_Enemy[i].matTranslation;
				m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
				m_pEnemyBoxMesh->DrawSubset(0);
			}
		}

		// ���ΰ� 	
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_sPlayer.matWorld);
		m_pMesh->DrawSubset(0);
		m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		SetRect(&rt, 10, 20, 0, 0);
		sprintf(string, "Total Score : %d   Score :  %d", m_nTotalGrade, m_nGrade);
		m_pFont3->DrawText(NULL, string, -1, &rt, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));

		SetRect(&rt, 550, 20, 0, 0);
		sprintf(string, "%s", "��");
		for (i = 0; i < m_sPlayer.nLife; i++)
		{
			rt.left = 680 + i * 15;
			m_pFont2->DrawText(NULL, string, -1, &rt, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		}
		break;
	case SUCCESS:
		SetRect(&rt, 220, 130, 0, 0);
		m_pFont->DrawText(NULL, "SUCCESS", -1, &rt, DT_NOCLIP,
			D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		break;
	case FAILED:
		SetRect(&rt, 220, 230, 0, 0);
		m_pFont2->DrawText(NULL, "�ٽ� �Ͻðڽ��ϱ�? (Yes/No)", -1, &rt, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		break;
	case END:
		SetRect(&rt, 260, 280, 0, 0);
		sprintf(string, "Total Score : %d", m_nTotalGrade);
		m_pFont2->DrawText(NULL, string, -1, &rt, DT_NOCLIP, D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
		break;
	}

}

void CGameApp::OnUpdate()
{
	int i, j;

	static DWORD OldTime;
	DWORD dwCurTime = GetTickCount();
	m_dwElapsedTime = dwCurTime - OldTime;
	OldTime = dwCurTime;
	D3DXVECTOR3 vMin1, vMax1, vMin2, vMax2;
	D3DXMATRIX matWorld;
	float fTemp;
	static DWORD dwReadyTime; // �غ� ��� �ð��� ������Ű�� ���ؼ�

							  // ���� ��� �ð� üũ �κ�
	m_dwGameElapsedTime = dwCurTime - m_dwGameStartTime;

	switch (m_nGameState)
	{
	case INIT:
		//if (GetAsyncKeyState(VK_SPACE) < 0)
		//{
			m_nGameState = READY;
			m_nStage++;
			dwReadyTime = 0;
			OnInit();
		//}
		break;
	
	case READY:
		dwReadyTime += m_dwElapsedTime;
		if (dwReadyTime >= 2000) //3�ʸ� �������� ����
		{
			m_nGameState = RUN;
		}
		break;
	case RUN:
		if (GetAsyncKeyState(VK_LEFT) < 0)
		{
			if (m_sPlayer.vPos.x - (m_dwElapsedTime * m_sPlayer.fVelocity) >= -6.0f)
				m_sPlayer.vPos.x -= m_dwElapsedTime * m_sPlayer.fVelocity;
		}

		if (GetAsyncKeyState(VK_RIGHT) < 0)
		{
			if (m_sPlayer.vPos.x + (m_dwElapsedTime * m_sPlayer.fVelocity) <= 6.0f)
				m_sPlayer.vPos.x += m_dwElapsedTime * m_sPlayer.fVelocity;
		}

		if (GetAsyncKeyState(VK_UP) < 0)
		{
			if (m_sPlayer.vPos.y + (m_dwElapsedTime * m_sPlayer.fVelocity) <= 10.0f)
				m_sPlayer.vPos.y += m_dwElapsedTime * m_sPlayer.fVelocity;
		}
		if (GetAsyncKeyState(VK_DOWN) < 0)
		{
			if (m_sPlayer.vPos.y - (m_dwElapsedTime * m_sPlayer.fVelocity) >=0.0f)
				m_sPlayer.vPos.y -= m_dwElapsedTime * m_sPlayer.fVelocity;
		}

		if (GetAsyncKeyState(VK_SPACE) < 0)  // ���ΰ� �̻��� �߻�
		{
			if (dwCurTime - m_sPlayer.dwOldBulletFireTime >= m_sPlayer.dwBulletFireTime)
			{
				m_sPlayer.dwOldBulletFireTime = dwCurTime;

				for (i = 0; i < 10; i++)
				{
					if (m_sPlayerBullet[i].nLife <= 0)
					{
						m_sPlayerBullet[i].nLife = 1;
						m_sPlayerBullet[i].vPos = m_sPlayer.vPos;
						m_sPlayerBullet[i].vPos.z += 1.0f;
						break;
					}
				}
			}
		}
		// ���ΰ��� �浹 �ڽ�
		matWorld = m_sPlayer.matScale * m_sPlayer.matRotationY * m_sPlayer.matTranslation;
		D3DXVec3TransformCoord(&vMin1, &m_sPlayer.vMin, &matWorld);
		D3DXVec3TransformCoord(&vMax1, &m_sPlayer.vMax, &matWorld);
		// �� ĳ���� ����
		for (i = m_nEnemyIndex; i < 100; i++)
		{
			if (m_Enemy[i].dwAppearTime <= m_dwGameElapsedTime)
			{
				m_Enemy[i].nLife = rand()%2+1;
				m_nEnemyIndex++;
			}
			else {
				break;
			}
		}

		// �� ĳ���� �̵��� �� ĳ���� �Ѿ� �߻�
		for (i = 0; i < m_nEnemyIndex; i++)
		{
			if (m_Enemy[i].nLife)
			{
				m_Enemy[i].vPos.z -= m_dwElapsedTime * m_EnemyProperty.fMoveVelcoty;
				if (m_Enemy[i].vPos.z <= -20.0f) // ��� ����
				{
					m_Enemy[i].nLife = 0;
					m_nEnemyCount--;    // �� ĳ������ ���� 0 �̵Ǹ� ���� 
					if (m_nEnemyCount == 0)
					{
						m_nGameState = STOP;
					}
					continue;
				}

				D3DXMatrixTranslation(&m_Enemy[i].matTranslation, m_Enemy[i].vPos.x, m_Enemy[i].vPos.y, m_Enemy[i].vPos.z);

				// Note: ��� �ִ� �� ĳ���͸��� �Ѿ��� �߻��� �� �����Ƿ�
				if (dwCurTime - m_Enemy[i].dwOldFireTime >= m_EnemyProperty.dwFireTime)
				{
					m_Enemy[i].dwOldFireTime = dwCurTime;
					for (j = 0; j < 100; j++)
					{
						if (!m_EnemyBullet[j].nLife)
						{
							m_EnemyBullet[j].nLife = 1;
							m_EnemyBullet[j].vPos = m_Enemy[i].vPos;
							m_EnemyBullet[j].vPos.z -= 0.5f; // �ణ �տ��� �Ѿ� �߻簡 �ǵ��� �ϱ� ����									
							break; // <-- �̰� �߿��ϴ�. �Ѿ� �ϳ��� �츮�� �κ��� �� 
						}
					}
				}
			}
		}

		// �� ĳ���� �Ѿ� �̵�
		for (i = 0; i < 100; i++)
		{
			if (m_EnemyBullet[i].nLife > 0)
			{
				m_EnemyBullet[i].vPos.z -= m_dwElapsedTime * m_EnemyBulletProperty.fBulletVelcoty;
				if (m_EnemyBullet[i].vPos.z <= -20.0f) // ��� ���� �浹
					m_EnemyBullet[i].nLife = 0;
				else
					D3DXMatrixTranslation(&m_EnemyBullet[i].matTranslation, m_EnemyBullet[i].vPos.x, m_EnemyBullet[i].vPos.y, m_EnemyBullet[i].vPos.z);
			}
		}

		// Note: ���ΰ� �Ѿ� �̵�
		for (i = 0; i < 10; i++)
		{
			if (m_sPlayerBullet[i].nLife > 0)
			{
				m_sPlayerBullet[i].vPos.z += m_dwElapsedTime * m_sPlayerBulletProperty.fBulletVelcoty;
				if (m_sPlayerBullet[i].vPos.z >= 20.0f) // ��� ���� �浹
					m_sPlayerBullet[i].nLife = 0;
				else
					D3DXMatrixTranslation(&m_sPlayerBullet[i].matTranslation, m_sPlayerBullet[i].vPos.x, m_sPlayerBullet[i].vPos.y, m_sPlayerBullet[i].vPos.z);
			}
		}

		// Note: ���ΰ��� �� ĳ���� �浹
		D3DXMatrixTranslation(&m_sPlayer.matTranslation, m_sPlayer.vPos.x, m_sPlayer.vPos.y, m_sPlayer.vPos.z);

		

		if (vMin1.x > vMax1.x)  // ȸ���� �����Ƿ� �ּ��� ���� �����Ѵ�.
		{
			fTemp = vMin1.x;
			vMin1.x = vMax1.x;
			vMax1.x = fTemp;
		}

		if (vMin1.y > vMax1.y)
		{
			fTemp = vMin1.y;
			vMin1.y = vMax1.y;
			vMax1.y = fTemp;
		}

		if (vMin1.z > vMax1.z)
		{
			fTemp = vMin1.z;
			vMin1.z = vMax1.z;
			vMax1.z = fTemp;
		}

		for (i = 0; i < m_nEnemyIndex; i++)
		{
			if (m_Enemy[i].nLife > 0)
			{
				D3DXMatrixTranslation(&m_Enemy[i].matTranslation, m_Enemy[i].vPos.x, m_Enemy[i].vPos.y, m_Enemy[i].vPos.z);
				matWorld = m_EnemyProperty.matScale * m_Enemy[i].matTranslation;

				D3DXVec3TransformCoord(&vMin2, &m_EnemyProperty.vMin, &matWorld);
				D3DXVec3TransformCoord(&vMax2, &m_EnemyProperty.vMax, &matWorld);

				if (CheckCubeIntersection(&vMin1, &vMax1, &vMin2, &vMax2))
				{
					m_Enemy[i].nLife = 0;
					m_sPlayer.nLife--;
					m_nEnemyCount--;    // �� ĳ������ ���� 0 �̵Ǹ� ���� 
					m_nGrade += 10; // ���ΰ��� �浹�ϸ鼭 ���� ���̹Ƿ� ���� ����
					
					if (m_sPlayer.nLife == 0)
					{
						m_nGameState = STOP;
					}

					if (m_nEnemyCount == 0)
					{
						m_nGameState = STOP;
					}
				}
			}
		}

		// Note: ���ΰ��� �� �Ѿ� �浹
		for (i = 0; i < 100; i++)
		{
			if (m_EnemyBullet[i].nLife > 0)
			{
				D3DXMatrixTranslation(&m_EnemyBullet[i].matTranslation, m_EnemyBullet[i].vPos.x, m_EnemyBullet[i].vPos.y, m_EnemyBullet[i].vPos.z);
				matWorld = m_EnemyBullet[i].matTranslation;
				D3DXVec3TransformCoord(&vMin2, &m_EnemyBulletProperty.vMin, &matWorld);
				D3DXVec3TransformCoord(&vMax2, &m_EnemyBulletProperty.vMax, &matWorld);

				if (CheckCubeIntersection(&vMin1, &vMax1, &vMin2, &vMax2))
				{
					m_EnemyBullet[i].nLife = 0;
					m_sPlayer.nLife--;
					if (m_sPlayer.nLife == 0)
					{
						m_nGameState = STOP;
					}
				}
			}
		}

		// Note: ���ΰ� �Ѿ˰� �� ĳ���� �浹
		for (i = 0; i < 10; i++)
		{
			if (m_sPlayerBullet[i].nLife > 0)
			{
				D3DXMatrixTranslation(&m_sPlayerBullet[i].matTranslation, m_sPlayerBullet[i].vPos.x, m_sPlayerBullet[i].vPos.y, m_sPlayerBullet[i].vPos.z);
				matWorld = m_sPlayerBulletProperty.matScale * m_sPlayerBullet[i].matTranslation;
				D3DXVec3TransformCoord(&vMin1, &m_sPlayerBulletProperty.vMin, &matWorld);
				D3DXVec3TransformCoord(&vMax1, &m_sPlayerBulletProperty.vMax, &matWorld);

				// Note: �� ĳ���Ϳ� �浹 üũ
				for (j = 0; j < m_nEnemyIndex; j++)
				{
					if (m_Enemy[j].nLife > 0)
					{
						D3DXMatrixTranslation(&m_Enemy[j].matTranslation, m_Enemy[j].vPos.x, m_Enemy[j].vPos.y, m_Enemy[j].vPos.z);
						matWorld = m_EnemyProperty.matScale * m_Enemy[j].matTranslation;

						D3DXVec3TransformCoord(&vMin2, &m_EnemyProperty.vMin, &matWorld);
						D3DXVec3TransformCoord(&vMax2, &m_EnemyProperty.vMax, &matWorld);

						if (CheckCubeIntersection(&vMin1, &vMax1, &vMin2, &vMax2))
						{
							m_Enemy[j].nLife = 0;
							m_sPlayerBullet[i].nLife = 0;
							m_nGrade += 10; // ���� ����

							m_nEnemyCount--;    // �� ĳ������ ���� 0 �̵Ǹ� ���� 
							if (m_nEnemyCount == 0)
							{
								m_nGameState = STOP;
							}
							break;
						}
					}
				}

				// Note: ���ΰ� �Ѿ˰� �� �Ѿ� �浹 üũ
				if (m_sPlayerBullet[i].nLife > 0)
				{
					for (j = 0; j < 100; j++)
					{
						if (m_EnemyBullet[j].nLife > 0)
						{
							D3DXMatrixTranslation(&m_EnemyBullet[j].matTranslation, m_EnemyBullet[j].vPos.x, m_EnemyBullet[j].vPos.y, m_EnemyBullet[j].vPos.z);
							matWorld = m_EnemyBullet[j].matTranslation;
							D3DXVec3TransformCoord(&vMin2, &m_EnemyBulletProperty.vMin, &matWorld);
							D3DXVec3TransformCoord(&vMax2, &m_EnemyBulletProperty.vMax, &matWorld);

							if (CheckCubeIntersection(&vMin1, &vMax1, &vMin2, &vMax2))
							{
								m_EnemyBullet[j].nLife = 0;
								m_sPlayerBullet[i].nLife = 0;
								break;
							}
						}
					}
				}
			}
		}

		D3DXMatrixTranslation(&m_sPlayer.matTranslation, m_sPlayer.vPos.x, m_sPlayer.vPos.y, m_sPlayer.vPos.z);
		m_sPlayer.matWorld = m_sPlayer.matScale * m_sPlayer.matRotationY * m_sPlayer.matTranslation;
		break;

	case STOP:
		if (m_sPlayer.nLife < 0 || m_nEnemyCount > 0)
		{
			m_nGameState = FAILED;
		}
		else {
			m_nGameState = SUCCESS;
		}

		dwReadyTime = 0;
		break;

	case SUCCESS:
		dwReadyTime += m_dwElapsedTime;
		if (dwReadyTime >= 3000) //3�ʸ� �������� ����
		{
			m_nGameState = READY;
			m_nStage++;
			m_nTotalGrade += m_nGrade;
			OnInit();
		}
		break;

	case FAILED:
		if (GetAsyncKeyState('Y') < 0)
		{
			m_nGameState = READY;
			m_nGrade = 0;
			dwReadyTime = 0;
			OnInit();
			break;
		}

		if (GetAsyncKeyState('N') < 0)
		{
			m_nGameState = END;
			dwReadyTime = 0;
			m_nTotalGrade += m_nGrade;
			break;
		}
		break;
	}
}

void CGameApp::OnRelease()
{
	m_pEnemyBoxMesh->Release();
	m_pEnemyBulletMesh->Release();
	m_pMesh->Release();
	m_pPlayerBulletMesh->Release();
	m_Ground.OnRelease();
}