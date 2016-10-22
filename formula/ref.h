/*
 * ref.h: 
 *
 * Copyright (C) 2003 Marcus Nilsson (marcusn@it.uu.se)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Authors:
 *    Marcus Nilsson (marcusn@it.uu.se)
 *    Based on ref class by Johann Deneux <johannd@it.uu.se>
 */

#ifndef RMC_FORMULA_REF_H
#define RMC_FORMULA_REF_H

#include <memory>

namespace rmc
{
	namespace formula
	{
		template <class T>
		class Ref
		{
			void ref()
				{
					if (count != NULL) (*count)++;
				}

			void unref()
				{
					if (count != NULL)
					{
						(*count)--;
						if (*count == 0)
						{
							//delete p;
							delete count;
						}
					}
				}
		protected:
			T* p;
			int* count;
		public:
			Ref():
				p(NULL),
				count(NULL)
				{}

			Ref(const Ref& r):
				p(r.p),
				count(r.count)
				{
					ref();
				}

			template <class T2>
			Ref(const Ref<T2>& r):
				p(static_cast<T*>(r.p)),
				count(r.count)
				{
					ref();
				}

			Ref& operator=(const Ref& r)
				{
					if (p == r.p) return *this;
					
					unref();

					p = r.p;
					count = r.count;

					ref();

					return *this;
				}

			template <class T2>
			Ref& operator=(const Ref<T2>& r)
				{
					if (p == r.p) return *this;
					
					unref();

					p = r.p;
					count = r.count;

					ref();

					return *this;
				}

			explicit Ref(T* p):
				p(p),
				count(new int(1))
				{}

			template <class T2>
			operator const T2&() const
				{
					return static_cast<const T2&>(*p);
				}

			T& operator*() const { return *p; }
			T* operator->() const { return p; }

			bool operator==(const Ref<T>& r) const
				{
					return p == r.p;
				}

			~Ref()
				{
					unref();
				}
					
		};

		template <class T>
		class Ident : public Ref<T>
		{
		public:
			Ident():
				Ref<T>()
				{}

			template <class T2>
			Ident(const Ident<T2>& r):
				Ref<T>(r)
				{}

			Ident(T* p):
				Ref<T>(p)
				{}
		};

		template <class T>
		class Shared : private Ref<T>
		{
		public:
			Shared(const Shared& r):
				Ref<T>(r)
				{}

			template <class T2>
			Shared(const Shared<T2>& r):
				Ref<T>(r)
				{}

			Shared(T* p):
				Ref<T>(p)
				{}

			Shared():
				Ref<T>()
				{}

			Shared& operator=(const Shared& r)
				{
					Ref<T>::operator=(r);
					return *this;
				}

			template <class T2>
			Shared& operator=(const Shared<T2>& r)
				{
					Ref<T>::operator=(r);
					return *this;
				}

			template <class T2>
			operator const T2&() const
				{
					return static_cast<const T2&>(*Ref<T>::p);
				}
			
			const T& operator*() const { return static_cast<const T&>(*Ref<T>::p); }
			T* operator->() const { return Ref<T>::operator->(); }

			bool operator==(const Shared<T>& r) const
				{
					if (Ref<T>::operator==(r)) return true;

					if (Ref<T>::p == NULL || r.p == NULL) return false;

					const T& n1 = *(*this);
					const T& n2 = *r;

					return  (n1 == n2);
				}
		};
	}
}
				


#endif /* REF_H */
