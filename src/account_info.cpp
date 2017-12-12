/**
 * @file    account_info.h
 * @brief   
 * @ref     
 * @author  Taehong, Kim (taehong@somma.kr)
 * @date    2017/12/12 created.
 * @copyright All rights reserved by Yonghwan, Roh.
**/
#include "stdafx.h"
#include "Win32Utils.h"
#include <sddl.h>
#include "account_info.h"

/// @brief	
account::account(_In_ const wchar_t * name, 
				 _In_ DWORD password_age,
				 _In_ DWORD privilege,
				 _In_ DWORD flags,
				 _In_ const wchar_t * script_path,
				 _In_ DWORD last_logon,
				 _In_ DWORD num_logons,
				 _In_ const wchar_t * sid) :
	_name(nullptr == name ? L"" : name),
	_password_age(password_age),
	_privilege(privilege),
	_flags(flags),
	_script_path(script_path),
	_last_logon(last_logon),
	_num_logons(num_logons),
	_sid(sid)
{}

/// @brief	���� Ÿ��(Administrator, User, Guest)
const wchar_t* account::privilege() const 
{
	switch (_privilege)
	{
	case USER_PRIV_ADMIN: return L"Administrator";
	case USER_PRIV_USER: return L"User";
	case USER_PRIV_GUEST: return L"Guest";
	default:
		_ASSERTE(!"Never reach here");
		return L"Unknown";
	}
}

/// @brief	���� �Ӽ�
std::wstring account::attribute_to_string()
{
	bool addlf = false;
	std::wstringstream flag;
	if (FlagOn(_flags, UF_SCRIPT))
	{
		flag << "UF_SCRIPT"; addlf = true;
	}
	if (FlagOn(_flags, UF_ACCOUNTDISABLE))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_ACCOUNTDISABLE";
	}
	if (FlagOn(_flags, UF_HOMEDIR_REQUIRED))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_HOMEDIR_REQUIRED";
	}
	if (FlagOn(_flags, UF_PASSWD_NOTREQD))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_PASSWD_NOTREQD";
	}
	if (FlagOn(_flags, UF_PASSWD_CANT_CHANGE))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_PASSWD_CANT_CHANGE";
	}
	if (FlagOn(_flags, UF_DONT_EXPIRE_PASSWD))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_DONT_EXPIRE_PASSWD";
	}
	if (FlagOn(_flags, UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED";
	}
	if (FlagOn(_flags, UF_NOT_DELEGATED))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_NOT_DELEGATED";
	}
	if (FlagOn(_flags, UF_SMARTCARD_REQUIRED))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_SMARTCARD_REQUIRED";
	}
	if (FlagOn(_flags, UF_USE_DES_KEY_ONLY))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_USE_DES_KEY_ONLY";
	}
	if (FlagOn(_flags, UF_DONT_REQUIRE_PREAUTH))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_DONT_REQUIRE_PREAUTH";
	}
	if (FlagOn(_flags, UF_TRUSTED_FOR_DELEGATION))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_TRUSTED_FOR_DELEGATION";
	}
	if (FlagOn(_flags, UF_PASSWORD_EXPIRED))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_PASSWORD_EXPIRED";
	}
	if (FlagOn(_flags, UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION))
	{
		if (true == addlf) { flag << L", "; }
		else { addlf = true; }
		flag << L"UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION";
	}
	return flag.str();
}

/// @brief	������ �α��� �ð��� �ѱ� �ð�(KST)�� ��ȯ�� ���ڿ�
std::wstring account::last_logon_kst()
{
	if (0 == _last_logon) return L"";

	//
	// time_t to filetime
	//
	FILETIME ft;
	LONGLONG ll = Int32x32To64(_last_logon, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;
	
	//
	// filetime to str
	//
	return MbsToWcsEx(file_time_to_str(&ft, true, false).c_str());
}









/// @brief `PSID`�� ���ڿ��� ��ȯ �Ѵ�. 
bool
psid_to_wstr_sid(
	_In_ PSID sid,
	_Out_ std::wstring& sid_string
	)
{
	_ASSERTE(nullptr != sid);
	if (nullptr == sid) return false;

	//
	//	ConvertSidToStringSidW() �� ������ ���۴� �ݵ�� LocalFree ��
	//	�Ҹ��ؾ� �Ѵ�.
	//
	wchar_t* sid_buf = nullptr;	
	wchar_ptr sid_ptr(sid_buf, [](wchar_t* p) 
	{ 
		if (nullptr != p) LocalFree(p); 
	});

	if (TRUE != ConvertSidToStringSidW(sid, &sid_buf))
	{
		DWORD gle = GetLastError();
		const char* gles = nullptr;
		switch (gle)
		{
		case ERROR_NOT_ENOUGH_MEMORY: gles = "ERROR_NOT_ENOUGH_MEMORY"; break;
		case ERROR_INVALID_SID:		  gles = "ERROR_INVALID_SID"; break;
		case ERROR_INVALID_PARAMETER: gles = "ERROR_INVALID_PARAMETER"; break;
		}

		if (nullptr != gles)
		{
			log_err "ConvertSidToStringSidW() failed. gle=%s",
				gles
				log_end;
			return false;
		}
		else
		{
			log_err "ConvertSidToStringSidW() failed. gle=%u",
				gle
				log_end;
			return false;
		}
	}

	sid_string = sid_buf;
	return true;
}

/// @brief  ���� �̸��� ������ ���� ������ ��ȸ�ؼ� ��ȯ�Ѵ�.
bool
get_account_info_by_name(
	_In_ wchar_t* user_name,
	_Out_ LPUSER_INFO_4* user_info
	)
{
	_ASSERTE(nullptr != user_name);
	if (nullptr == user_name) return false;

	//
	// NetUserGetInfo �Լ����� `infomation level`�� ���� �о� �� �� �ִ�
	// ����� ������ �ٸ���. ������� �� ������ �о� ���°� 3 �Ǵ� 4���� 
	// �̸� ��� �ϰ�, MSDN�� ���ϸ� level 3���ٴ� 4�� ����ϴ°� ���� �ϰ�
	// �ִ�.
	// ���� URL : https://msdn.microsoft.com/ko-kr/library/windows/desktop/aa370654(v=vs.85).aspx
	//
	DWORD status = NetUserGetInfo(NULL,
								  user_name,
								  4,
								  (LPBYTE*)user_info);

	if (NERR_Success != status)
	{
		const char* status_str = nullptr;
		switch (status)
		{
		case ERROR_ACCESS_DENIED:  status_str = "ERROR_ACCESS_DENIED"; break;
		case ERROR_BAD_NETPATH:    status_str = "ERROR_BAD_NETPATH"; break;
		case ERROR_INVALID_LEVEL:  status_str = "ERROR_INVALID_LEVEL"; break;
		case NERR_InvalidComputer: status_str = "NERR_InvalidComputer"; break;
		case NERR_UserNotFound:    status_str = "NERR_UserNotFound"; break;
		}

		if (nullptr != status_str)
		{
			log_err
				"GetLocalAccountInfos::NetUserGetInfo failed. status=%s",
				status_str
				log_end;
		}
		{
			log_err
				"GetLocalAccountInfos::NetUserGetInfo failed. status=%d",
				status
				log_end;
		}

		return false;
	}

	return true;
}

/// @brief  �ý����� ��� ���� ������ �о� �´�.
bool
get_account_infos(
	_Out_ std::list<paccount>& accounts
	)
{
	uint8_t* buffer = nullptr;
	DWORD entries_read, total_entries;
	DWORD resume_handle = 0;
	NET_API_STATUS ret;
	do
	{
		//
		// �ý����� ��� ���� ������ �д´�. �̶� ���� ���� ���� ������ `level` 
		// ���ڰ����� ���� �����ϸ� ���� ������ iterate�� �� ����� ���� Ÿ�Ժ�
		// ���͸� �� �� �ִ�. ���� �ʿ��� ����� ����� ������ ��ġ�� ���� �Ǿ� �ִ� ���� 
		// �� ����ڰ� ������ ������ �о� ������(`FILTER_NORMAL_ACCOUNT`) �Ǿ� �ִ�.
		// ���� : `Active Directory`ȯ�濡�� ���α׷��� ���� �� �� ACL�� ���ؼ�
		//       `NetUserEnum`�� �̿��� ���� ��ȸ�� ��� �ǰų� �ź� �� �� �ִ�.
		//       ACL �⺻ ��å�� ���� ����� �׸��� `Pre-Windows 2000 compat-
		//       ible access` �׷쿡 ���� �������� ���α׷��� ���� �ȴٸ� ���� ��
		//       ȸ�� ������ ����.
		// ���� : https://msdn.microsoft.com/ko-kr/library/windows/desktop/aa370652(v=vs.85).aspx
		//
		ret = NetUserEnum(NULL,
						  0,
						  FILTER_NORMAL_ACCOUNT,
						  &buffer,
						  MAX_PREFERRED_LENGTH,
						  &entries_read,
						  &total_entries,
						  &resume_handle);
		if ((NERR_Success != ret) && (ERROR_MORE_DATA != ret))
		{
			log_err
				"NetUserEnum Error. NetApiStatus=%u",
				ret
				log_end;
			break;
		}
		else
		{
			USER_INFO_0* user_info_0 = (USER_INFO_0*)buffer;
			//
			// �ý��ۿ� �ִ� ������ iterate�ϸ鼭 ���� ������ �����´�.
			//
			for (DWORD count = 0; count < total_entries; count++)
			{
				//
				// �������� �̿��ؼ� ������ ���� �� ������ ȹ���Ѵ�.
				//
				LPUSER_INFO_4 user_info = NULL;
				if (!get_account_info_by_name(user_info_0->usri0_name,
											  &user_info))
				{
					log_err
						"get_user_info_by_name failed. name(%ws)",
						user_info_0->usri0_name
						log_end;
					user_info_0++;
					continue;
				}

				//
				// psid�� ���ڿ� sid�� ��ȯ�� �Ѵ�.
				// 
				std::wstring sid_str;
				psid_to_wstr_sid(user_info->usri4_user_sid, sid_str);
				paccount ac = new account(user_info->usri4_name,
										  user_info->usri4_password_age,
										  user_info->usri4_priv,
										  user_info->usri4_flags,
										  user_info->usri4_script_path,
										  user_info->usri4_last_logon,
										  user_info->usri4_num_logons,
										  sid_str.c_str());
				accounts.push_back(ac);

				if (user_info) { NetApiBufferFree(user_info); user_info = nullptr; }

				user_info_0++;
			}
		}
		if (buffer) { NetApiBufferFree(buffer); buffer = nullptr; }
	} while (ERROR_MORE_DATA == ret);

	if (buffer) { NetApiBufferFree(buffer); buffer = nullptr; }
	return true;
}